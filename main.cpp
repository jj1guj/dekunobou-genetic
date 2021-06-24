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

int main(int argc,char** argv){
    int threads=0;
    if(argc>1)threads=atoi(argv[1]);

    load_eval("eval.txt",param);
    //人との対決用
    play_engine_human(param);
}
