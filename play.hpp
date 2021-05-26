#include"dekunobou.hpp"
#include"othello.hpp"
#include"engine.hpp"

#pragma once
void play_with_human(){
    Board board;
    std::string turn[2]={"Black", "White"};
    int pass_count=0;
    int move;
    while(true){
        if(pass_count>1)break;
        LegalMoveList moves(board);
        if(moves.size()==0){
            std::cout<<"pass\n";
            ++pass_count;
            continue;
        }
        pass_count=0;
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

int play_engine(float param_black[param_size],float param_white[param_size]){
    Board board;
    int pass_count=0;
    int move;
    while(true){
        if(pass_count>1)break;
        LegalMoveList moves(board);

        if(moves.size()==0){
            board.push(-1);
            ++pass_count;
            continue;
        }
        pass_count=0;

        if(board.turn){
            //後手番
            move=go(board,param_white);
        }else{
            //先手番
            move=go(board,param_black);
        }
        //着手
        board.push(move);
    }

    if(board.point[0]>board.point[1])return 0;
    else if(board.point[0]<board.point[1])return 1;
    else return 2;
}