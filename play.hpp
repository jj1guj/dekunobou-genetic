#include"dekunobou.hpp"
#include"othello.hpp"
#include"engine.hpp"

#pragma once
int play_engine_human(){
    Board board;
    std::string turn[2]={"Black", "White"};
    std::string s;
    int pass_count=0;
    int move;
    bool turn_engine;

    std::cout<<"select your turn[b/w]: ";
    std::cin>>s;
    if(s=="b")turn_engine=true;
    else turn_engine=false;

    while(true){
        if(pass_count>1)break;
        LegalMoveList moves(board);
        if(moves.size()==0){
            std::cout<<"pass\n";
            board.push(-1);
            ++pass_count;
            continue;
        }
        pass_count=0;
        if(board.turn==turn_engine){
            disp(board);
            board.push(go(board));
        }else{
            disp_teban(board);
            std::cout<<"input where to put: ";
            std::cin>>move;
            while(move>moves.size()){
                std::cout<<"error. try again\n";
                std::cout<<"input where to put: ";
                std::cin>>move;
            }
            board.push(moves[move-1]);
        }
    }
    disp(board);
    std::cout<<"black: "<<board.point[0]<<"vs"<<board.point[1]<<" :white\n";
    if(board.point[0]>board.point[1]){
        std::cout<<"black win\n";
        return 0;
    }else if(board.point[0]<board.point[1]){
        std::cout<<"white win\n";
        return 1;
    }else{
        std::cout<<"draw\n";
        return 2;
    }
}