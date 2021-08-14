#include"play.hpp"
#include"experiment.hpp"
#include<fstream>
#include<istream>
#include<vector>
#include<omp.h>
#include<sstream>

float param[param_size]={
    -0.0550302,
0.524555,
-0.39094,
-0.0550302,
-0.0536549,
-0.915718,
0.550613,
0.97611,
-0.417893,
-0.598525,
-0.39094,
-0.00382641,
-0.417893,
0.656191,
-0.17878,
-0.212773,
-0.247796,
0.389348,
-0.451475,
-0.39094,
-0.758435,
0.559448,
0.796847,
-0.17878,
0.306573,
0.545377,
-0.201647,
-0.231373,
0.112637,
0.0118705,
-0.280754,
0.603965,
0.161015,
0.160384,
0.299399,
-0.791582,
0.0448458,
-0.791582,
0.161015,
-0.929401,
0.734466,
0.40045,
0.698171,
0.229479,
0.229479,
-0.582127,
0.0536363,
-0.495307,
-0.613028,
0.500293,
0.310196,
0.386627,
0.386627,
0.026728,
0.026728,
0.331291,
0.301926,
0.28042,
0.386627,
0.725266,
-0.887571,
-0.222232,
0.0150876,
-0.315732,
-0.352032,
-0.352846,
-0.393754,
0.706992,
-0.0711386,
-0.352032,
-0.0142896,
-0.142079,
-0.55012,
0.627141,
-0.445127,
-0.498968,
0.145975,
-0.525881,
0.727475,
0.0711298,
-0.529421,
0.1218,
-0.101715,
0.1218,
-0.411867,
0.112244,
0.112244,
-0.555693,
-0.961099,
0.987055,
-0.0262266,
-0.0766981,
0.577391,
-0.101715,
-0.0262266,
-0.618373,
-0.684518,
0.923491,
-0.684518,
0.644187,
0.644187,
-0.98824,
-0.613063,
-0.98824,
0.631867,
-0.781754,
0.730877,
-0.98824,
-0.382372,
-0.237921,
0.94175,
0.465475,
0.496857,
-0.158435,
0.635292,
0.164941,
0.635292,
0.484589,
0.733831,
0.733831,
0.0535935,
-0.948024,
0.923273,
-0.50689,
0.928241,
0.781487,
0.713239,
-0.902039,
-0.50689,
0.750506,
-0.50689,
0.450574,
0.182597,
0.926912,
-0.254442,
-0.438542,
-0.975459,
0.775478,
-0.895759,
0.00256583,
0.923289,
-0.999735,
0.555328,
0.0461049,
0.555328,
0.520538,
0.923289,
-0.476344,
-0.30353,
-0.476344,
0.33921,
0.520538,
-0.745703,
-0.763018,
-0.516058,
-0.704378,
-0.892441,
-0.745703,
-0.148789,
-0.00259644,
0.969023,
0.950445,
-0.94028,
-0.01718,
-0.0384724,
0.950445,
-0.962322,
-0.388867,
0.785387,
-0.397765,
0.0604379,
-0.0131475,
-0.0131475,
0.925872,
0.943205,
0.371867,
0.639557,
0.371867,
-0.0513941,
0.00386099,
-0.153777,
-0.690303,
0.818791,
0.53299,
-0.153777,
-0.235918,
-0.448941,
-0.448941,
-0.568802,
-0.3191,
-0.409127,
0.291365,
0.695831,
-0.96913,
0.232993,
-0.538736,
0.588569,
0.168595,
-0.390954,
-0.454666,
0.350337,
-0.0781673,
0.344503,
-0.96913,
0.92937,
-0.858625,
0.679744,
0.679744,
-0.858625,
-0.858625,
0.679744,
0.555713,
-0.941428,
0.555713,
-0.714281,
0.0364865,
-0.295526,
-0.752591,
0.183157,
-0.891681,
-0.0513598,
-0.0259432,
0.939601,
-0.433767,
-0.358515,
0.712107,
-0.676271,
0.271573,
0.920429,
0.920429,
-0.492192,
-0.00901026,
0.947351,
0.130096,
0.759968,
0.0147756,
-0.506194,
0.504428,
-0.357816,
-0.567017,
0.995133,
0.270163,
0.157051,
0.524555,
-0.465383,
-0.465383
};

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
    //load_eval("eval.txt",param);//あとでソースコード中に埋め込む

    std::cout<<go(board,param)<<std::endl;
}
