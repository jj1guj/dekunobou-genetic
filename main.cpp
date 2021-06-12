#include"play.hpp"
#include"experiment.hpp"
#include<fstream>
#include<istream>
#include<vector>
#include<omp.h>
#include<sstream>
#define N 1024
using std::swap;

float params[N][param_size];
int memsize=sizeof(params[0]);

float param_black[param_size],param_white[param_size];
int result[3],win_impossible[1000];
ll win_count[N];

//遺伝的アルゴリズム関連のパラメータをグローバル変数として定義
int M,match_genetic,thresh,match_times;
float alpha;
double timelimit;
ll itr;
bool cur_used[N];
std::random_device rnd;

void init_param(float params[param_size]){
    std::random_device rnd;
    for(int i=0;i<param_size;++i){
        params[i]=2.0*(float)rnd()/0xffffffff-1.0;
    }
}

//対局用の評価関数の読み込み
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

//遺伝的アルゴリズムで使用したパラメータすべてをCSVファイルから読み込む
int load_params(std::string filename,float params[N][param_size]){
    std::ifstream inputs(filename);
    std::string s;
    int i=0,j;
    if(inputs.fail()){
        std::cout<<"Failed to open file\n";
        return -1;
    }
    while(getline(inputs,s)){
        std::stringstream ss{s};
        std::string buf;
        j=0;
        while(std::getline(ss,buf,',')){
            params[i][j]=std::stof(buf);
            ++j;
        }
        ++i;
    }
    return 0;
}

//交叉(並列化したいので関数化する)
void intersection(float p1[param_size],float p2[param_size],int cur1,int cur2){
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

    //遺伝子をもとに戻す
    cur_used[cur1]=false;
    cur_used[cur2]=false;
    memcpy(params[cur1],p1,memsize);
    memcpy(params[cur2],p2,memsize);
}

//盤面に対し線対称な点との平均を取る
void averaging(float dst[param_size],float src[param_size]){
    memcpy(dst,src,memsize);
    for(int i=0;i<64;++i){
        dst[i]=src[i];
        dst[i]+=src[i%8*8+i/8];
        dst[i]+=src[i/8*8+abs(i%8-7)];
        dst[i]+=src[i/8+abs(i%8-7)*8];
        dst[i]+=src[abs(i/8-7)*8+i%8];
        dst[i]+=src[abs(i/8-7)+i%8*8];
        dst[i]+=src[abs(i/8-7)*8+abs(i%8-7)];
        dst[i]+=src[abs(i/8-7)+abs(i%8-7)*8];
        dst[i]/=8;
    }
}
int main(int argc,char** argv){
    int threads=0;
    //人との対決用
    play_engine_human(params[0]);
}
