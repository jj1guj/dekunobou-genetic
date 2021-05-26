#include"play.hpp"
#define N 8192
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

int main(){
    std::random_device rnd;
    //パラメータの初期化
    for(int i=0;i<N;++i)init_param(params[i]);

    int M=100;//1世代での交叉回数
    int match_times=100;//対局回数
    float alpha=1e-2;//突然変異を起こす確率
    double timelimit=60;//単位は秒
    timelimit*=1000.0;//ミリ秒に変換
    ll itr=0;

    //遺伝的アルゴリズム
    std::chrono::system_clock::time_point start,end;
    start=std::chrono::system_clock::now();

    int cur1,cur2,l,r;
    int win_val[2],r1,r2;
    float c;
    float g1[param_size],g2[param_size],c1[param_size],c2[param_size];

    while(true){
        ++itr;
        std::cout<<"itr: "<<itr<<std::endl;
        
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
                    c=(float)rnd()/0xffffffff;
                    while(c==c1[i])c=(float)rnd()/0xffffffff;
                    c1[i]=c;
                }

                if((float)rnd()/0xffffffff<=alpha){
                    c=(float)rnd()/0xffffffff;
                    while(c==c2[i])c=(float)rnd()/0xffffffff;
                    c2[i]=c;
                }
            }

            //子と親で対戦する(ここ並列化したい)
            win_val[0]=0;
            win_val[1]=0;
            for(int b=0;b<match_times;++b){
                if(b%2==0){
                    r1=play_engine(g1,c1);
                    r2=play_engine(g2,c2);
                    if(r1==1)++win_val[0];
                    if(r2==1)++win_val[1];
                }else{
                    r1=play_engine(c1,g1);
                    r2=play_engine(c2,g2);
                    if(r1==0)++win_val[0];
                    if(r2==0)++win_val[1];
                }
            }

            //子の方が勝ち越したら置き換える
            if(win_val[0]>match_times/2){
                for(int i=0;i<param_size;++i)g1[i]=c1[i];
            }

            if(win_val[1]>match_times/2){
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
    for(int i=0;i<N-1;++i)for(int j=i+1;j<N;++j){
        for(int k=0;k<match_times;++k){
            if(k%2==0){
                winner=play_engine(params[i],params[j]);
                if(winner==0)++win_count[i];
                else if(winner==1)++win_count[j];
            }else{
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
    for(int i=0;i<param_size;++i)std::cout<<params[best][i]<<std::endl;


    //対局にかかる時間を計測する(100局やってその平均を取る)
    /*std::chrono::system_clock::time_point start,end;
    
    start=std::chrono::system_clock::now();
    for(int i=0;i<100;++i)++result[play_engine(param_black,param_white)];
    end=std::chrono::system_clock::now();
    double elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout<<elapsed<<" "<<elapsed/100.0<<std::endl;
    std::cout<<result[0]<<" "<<result[1]<<" "<<result[2]<<std::endl;*/
}