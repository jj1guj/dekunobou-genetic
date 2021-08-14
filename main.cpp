#include"play.hpp"
#include"experiment.hpp"
#include<fstream>
#include<istream>
#include<vector>
#include<omp.h>
#include<sstream>

float param[param_size];

//対局用の評価関数の読み込み
int load_eval(std::string filename,float param[param_size]){
    std::ifstream inputs(filename);
    std::string s;
    int i=0;
    if(inputs.fail()){
        std::cout<<"-6\n";
        return -6;
    }
    while(getline(inputs,s)){
        param[i]=std::stof(s);
        //std::cout<<param[i]<<std::endl;
        ++i;
    }
    return 0;
}

int main(int argc,char** argv){
    Board board;
    //要求通りの入力じゃなかったら負の数を出力
    if(argc!=3){
        std::cout<<"-1\n";
        return -1;
    }

    if(strlen(argv[1])!=64){
        std::cout<<"-2\n";
        return -2;
    }

    if(strlen(argv[2])!=1){
        std::cout<<"-3\n";
        return -3;
    }

    if(argv[2][0]!='0'&&argv[2][0]!='1'){
        std::cout<<"-4\n";
        return -4;
    }

    //盤面の文字列に規約違反の文字が含まれていないかみる
    for(int i=0;i<64;++i)if(argv[1][i]!='0'&&argv[1][i]!='1'&&argv[1][i]!='2'){
        std::cout<<"-5\n";
        return -5;
    }

    //盤をセット
    board.point[0]=0;
    board.point[1]=0;
    for(int i=0;i<64;++i){
        if(argv[1][i]=='0'){
            board.board[i/8][i%8]=0;
        }else if(argv[1][i]=='1'){
            board.board[i/8][i%8]=1;
            ++board.point[0];
        }else{
            board.board[i/8][i%8]=-1;
            ++board.point[1];
        }
    }

    //手番をセット
    if(argv[2]=="0"){
        board.turn=false;
    }else if(argv[2]=="1"){
        board.turn=true;
    }

    //評価関数の読み込み(仮)
    load_eval("eval.txt",param);//あとでソースコード中に埋め込む

    std::cout<<go(board,param)<<std::endl;
}
