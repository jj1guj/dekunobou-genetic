#include"play.hpp"
#include"experiment.hpp"
#include<fstream>
#include<vector>
#include<omp.h>
#define N 1024
using std::swap;

float params[N][param_size];

float param_black[param_size],param_white[param_size];
int result[3],win_impossible[1000];
ll win_count[N];

//遺伝的アルゴリズム関連のパラメータをグローバル変数として定義
int M,match_genetic,thresh,match_times;
float alpha;
double timelimit;
ll itr;
std::random_device rnd;

void init_param(float params[param_size]){
    std::random_device rnd;
    for(int i=0;i<param_size;++i){
        params[i]=2.0*(float)rnd()/0xffffffff-1.0;
    }
}

int load_eval(std::string filename,float param[param_size]){
    std::ifstream inputs(filename);
    std::string s;
    int i=0;
    if(inputs.fail()){
        std::cout<<"Failed to openfile\n";
        return -1;
    }
    while(getline(inputs,s)){
        param[i]=std::stof(s);
        std::cout<<param[i]<<std::endl;
        ++i;
    }
    return 0;
}

//交叉(並列化したいので関数化する)
void intersection(float p1[param_size],float p2[param_size]){
    std::random_device rnd;
    int win_val[2];
    float c,c1[param_size],c2[param_size];

    //M回交叉する
    for(int m=0;m<M;++m){
        int l=rnd()%param_size;
        int r=rnd()%param_size;
        while(l==r)r=rnd()%param_size;
        if(r<l)swap(r,l);
        for(int i=0;i<param_size;++i){
            if(l<=i&&i<=r){
                c1[i]=p2[i];
                c2[i]=p1[i];
            }else{
                c1[i]=p1[i];
                c2[i]=p2[i];
            }
            //確率に応じて突然変異を行う
            if((float)rnd()/0xffffffff<=alpha){
                c=2.0*(float)rnd()/0xffffffff-1.0;
                while(c==c1[i])c=2.0*(float)rnd()/0xffffffff-1.0;
                c1[i]=c;
            }

            if((float)rnd()/0xffffffff<=alpha){
                c=2.0*(float)rnd()/0xffffffff-1.0;
                while(c==c2[i])c=2.0*(float)rnd()/0xffffffff-1.0;
                c2[i]=c;
            }
        }

        //子と親で対戦する(ここ並列化したい)
        win_val[0]=0;
        win_val[1]=0;
        for(int b=0;b<match_genetic;++b){
            if(win_impossible[b]<=win_val[0]&&win_val[0]<=thresh){
                if(play_engine(p1,c1)==1)++win_val[0];
                if(play_engine(c1,p1)==0)++win_val[0];
            }
            if(win_impossible[b]<=win_val[1]&&win_val[1]<=thresh){
                if(play_engine(p2,c2)==1)++win_val[1];
                if(play_engine(c2,p2)==0)++win_val[1];
            }
        }

        //子の勝ち数が閾値を超えたら置き換える
        if(win_val[0]>thresh){
            for(int i=0;i<param_size;++i)p1[i]=c1[i];
        }

        if(win_val[1]>thresh){
            for(int i=0;i<param_size;++i)p2[i]=c2[i];
        }
    }
}

int main(int argc,char** argv){
    int threads=0;
    if(argc>1)threads=atoi(argv[1]);

    std::ofstream eval_output("eval.txt");

    //パラメータの初期化
    for(int i=0;i<N;++i)init_param(params[i]);

    //load_eval("eval.txt",params[0]);
    //rating_test(params[0],params[1]);
    //人との対決用
    //play_engine_human(params[0]);

    //for debug
    /*std::chrono::system_clock::time_point start,end;
    start=std::chrono::system_clock::now();
    for(int i=0;i<100;++i)play_engine(params[0],params[1]);
    end=std::chrono::system_clock::now();
    double elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout<<elapsed<<" "<<elapsed/100<<std::endl;*/

    

    M=100;//1世代での交叉回数
    match_genetic=30;//評価時の対局数
    thresh=0.55*match_genetic;//勝数がこの値を超えたら採用
    match_genetic/=2;
    
    match_times=50;//対局回数
    match_times/=2;
    alpha=1e-2;//突然変異を起こす確率
    timelimit=10*3600;//単位は秒
    timelimit*=1000.0;//ミリ秒に変換
    itr=0;

    //遺伝的アルゴリズム
    //なんか関数化して対局モードと切り替えられるようにしたい
    std::chrono::system_clock::time_point start,end;
    start=std::chrono::system_clock::now();

    int cur1,cur2,l,r;
    int r1,r2;
    //thresh勝不可能ラインの前計算(条件分岐を奇数局目でしか行わないのでその分だけ計算)
    for(int i=0;i<match_genetic/2;++i){
        win_impossible[i]=thresh+2*(i-match_genetic);
    }
    
    //並列化用に準備
    int concurrency=std::min(omp_get_max_threads(),N);
    if(threads>0)concurrency=std::min(concurrency,threads);
    std::cout<<"Concurrency: "<<concurrency<<std::endl;
    float G[256][param_size];
    int cursors[256],cur_now;
    bool cur_used[N];
    for(int i=0;i<N;++i)cur_used[i]=false;
    int memsize=sizeof(params[0]);

    while(true){
        ++itr;
        std::cout<<"Generation: "<<itr<<std::endl;
        
        //ランダムにthread数×2個体選び出しコピーする
        for(int i=0;i<concurrency*2;++i){
            cursors[i]=rnd()%N;
            while(cur_used[cursors[i]])cursors[i]=rnd()%N;
            cur_used[cursors[i]]=true;
            
            memcpy(G[i],params[cursors[i]],memsize);
        }
        for(int i=0;i<concurrency*2;++i)std::cout<<cursors[i]<<" ";
        std::cout<<std::endl;

        //交叉を並列に実行
        #pragma omp parallel for num_threads(concurrency)
        for(int i=0;i<concurrency;++i){
            intersection(G[2*i],G[2*i+1]);
        }

        //遺伝子をもとに戻す
        for(int i=0;i<concurrency*2;++i){
            cur_used[cursors[i]]=false;
            memcpy(params[cursors[i]],G[i],memsize);
        }

        if(itr%10==0){
            end=std::chrono::system_clock::now();
            double elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            std::cout<<itr<<" elapsed:"<<elapsed/1000<<std::endl<<std::endl;
            if(elapsed>timelimit)break;
        }
    }


    //総当たり戦を行い最終的に1番強いパラメータを出力
    int winner;
    for(int i=0;i<N;++i)win_count[i]=0;
    for(int i=0;i<N-1;++i){
        std::cout<<i<<std::endl;
        for(int j=i+1;j<N;++j){
            for(int k=0;k<match_times;++k){
                winner=play_engine(params[i],params[j]);
                if(winner==0)++win_count[i];
                else if(winner==1)++win_count[j];

                winner=play_engine(params[j],params[i]);
                if(winner==0)++win_count[j];
                else if(winner==1)++win_count[i];
            }
        }
    }

    int win_max=0;
    int best;
    for(int i=0;i<N;++i){
        if(win_count[i]>win_max){
            best=i;
            win_max=win_count[i];
        }
    }

    std::cout<<win_max<<"/"<<(N-1)*match_times*2<<std::endl;
    //output to file
    for(int i=0;i<param_size;++i)eval_output<<params[best][i]<<std::endl;
    eval_output.close();
}
