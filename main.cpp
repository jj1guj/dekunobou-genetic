#include"play.hpp"
//#include"experiment.hpp"
#include<fstream>
#define N 1024
using std::swap;

float params[N][param_size];

float param_black[param_size],param_white[param_size];
int result[3];
ll win_count[N];

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

int main(){
    std::random_device rnd;
    std::ofstream eval_output("eval.txt");

    //パラメータの初期化
    for(int i=0;i<N;++i)init_param(params[i]);

    //load_eval("eval",params[0]);
    //rating_test(params[0],params[1]);

    //for debug
    //play_engine(params[0],params[1]);
    

    int M=100;//1世代での交叉回数
    int match_genetic=30;//評価時の対局数
    int thresh=0.55*match_genetic;//勝数がこの値を超えたら採用
    match_genetic/=2;
    
    int match_times=50;//対局回数
    match_times/=2;
    float alpha=1e-2;//突然変異を起こす確率
    double timelimit=24*3600;//単位は秒
    timelimit*=1000.0;//ミリ秒に変換
    ll itr=0;

    //遺伝的アルゴリズム
    //なんか関数化して対局モードと切り替えられるようにしたい
    std::chrono::system_clock::time_point start,end;
    start=std::chrono::system_clock::now();

    int cur1,cur2,l,r;
    int win_val[2],r1,r2;
    float c;
    float g1[param_size],g2[param_size],c1[param_size],c2[param_size];

    while(true){
        ++itr;
        std::cout<<"Generation: "<<itr<<std::endl;
        
        //ランダムに2個体を選び出し、コピーする
        cur1=rnd()%N;
        cur2=rnd()%N;
        while(cur1==cur2)cur2=rand()%N;
        for(int i=0;i<param_size;++i){
            g1[i]=params[cur1][i];
            g2[i]=params[cur2][i];
        }

        //M回交叉する
        for(int m=0;m<M;++m){
            l=rnd()%param_size;
            r=rnd()%param_size;
            while(l==r)r=rnd()%param_size;
            if(r<l)swap(r,l);
            for(int i=0;i<param_size;++i){
                if(l<=i&&i<=r){
                    c1[i]=g2[i];
                    c2[i]=g1[i];
                }else{
                    c1[i]=g1[i];
                    c2[i]=g2[i];
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
                if(win_val[0]<=thresh){
                    if(play_engine(g1,c1)==1)++win_val[0];
                    if(play_engine(c1,g1)==0)++win_val[0];
                }
                if(win_val[1]<=thresh){
                    if(play_engine(g2,c2)==1)++win_val[1];
                    if(play_engine(c2,g2)==0)++win_val[1];
                }
            }

            //子の勝ち数が閾値を超えたら置き換える
            if(win_val[0]>thresh){
                for(int i=0;i<param_size;++i)g1[i]=c1[i];
            }

            if(win_val[1]>thresh){
                for(int i=0;i<param_size;++i)g2[i]=c2[i];
            }
        }

        //g1, g2をもとに戻す
        for(int i=0;i<param_size;++i){
            params[cur1][i]=g1[i];
            params[cur2][i]=g2[i];
        }

        if(itr%10==0){
            end=std::chrono::system_clock::now();
            double elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            std::cout<<itr<<" "<<elapsed<<std::endl;
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

    std::cout<<win_max<<std::endl;
    //output to file
    for(int i=0;i<param_size;++i)eval_output<<params[best][i]<<std::endl;
    eval_output.close();
}