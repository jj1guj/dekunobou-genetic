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

int play_engine_human(float param[param_size]){
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
            std::string s;
            //std::cout<<"pass[Y/n] ";
            //std::cin>>s;
            board.push(-1);
            ++pass_count;
            continue;
        }
        pass_count=0;
        if(board.turn==turn_engine){
            //disp(board);
            disp_teban(board);
            board.push(go(board,param));
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

int play_engine(float param_black[param_size],float param_white[param_size]){
    Board board;
    int pass_count=0;
    int move;
    while(true){
        //for debug
        //disp(board);
        
        if(pass_count>1)break;
        LegalMoveList moves(board);

        if(moves.size()==0){
            board.push(-1);
            ++pass_count;
            continue;
        }
        pass_count=0;

        disp_teban(board);
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

    disp(board);
    std::cout<<"black: "<<board.point[0]<<"vs"<<board.point[1]<<" :white\n";
    if(board.point[0]>board.point[1])return 0;
    else if(board.point[0]<board.point[1])return 1;
    else return 2;
}