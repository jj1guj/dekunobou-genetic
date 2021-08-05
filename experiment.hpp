#include"play.hpp"
#include<fstream>

/*void rating_test(float param1[param_size],float param2[param_size]){
    //レーティング計算による評価
    float rating[2]={1500.0,1500.0};
    float K=16;
    float win_rate1;
    int winner;
    ll win_count[2]={0,0};
    std::ofstream output("test_rating.csv");
    for(int i=1;i<=10000;++i){
        win_rate1=1/(powf(10.0,(rating[0]-rating[1])/400.0)+1);
        if(i%2==1){
            winner=play_engine(param1,param2);
            if(winner==0){
                rating[0]+=K*win_rate1;
                rating[1]-=K*win_rate1;
                ++win_count[0];
            }else if(winner==1){
                rating[0]-=K*(1-win_rate1);
                rating[1]+=K*(1-win_rate1);
                ++win_count[1];
            }
        }else{
            winner=play_engine(param2,param1);
            if(winner==0){
                rating[0]-=K*(1-win_rate1);
                rating[1]+=K*(1-win_rate1);
                ++win_count[1];
            }else if(winner==1){
                rating[0]+=K*win_rate1;
                rating[1]-=K*win_rate1;
                ++win_count[0];
            }
            output<<i<<","<<rating[0]<<","<<rating[1]<<","<<(float)win_count[1]/i<<","<<1/(powf(10.0,(rating[0]-rating[1])/400.0)+1)<<","<<(float)win_count[1]/i-1/(powf(10.0,(rating[0]-rating[1])/400.0)+1)<<std::endl;
            std::cout<<i<<","<<rating[0]<<","<<rating[1]<<","<<(float)win_count[1]/i<<","<<1/(powf(10.0,(rating[0]-rating[1])/400.0)+1)<<","<<(float)win_count[1]/i-1/(powf(10.0,(rating[0]-rating[1])/400.0)+1)<<std::endl;
        }
    }
}*/