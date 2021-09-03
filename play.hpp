#include"dekunobou.hpp"
#include"othello.hpp"
#include"engine.hpp"

#pragma once
int play_engine(float param_black[param_size],float param_white[param_size]){
    Board board;
    int move;
    while(!board.is_over()){
        //for debug
        //disp(board);
        if(board.is_pass()){
            board.push(-1);
            continue;
        }

        if(board.point[0]+board.point[1]<=24)cur_offset=cur_opening;
        else if(board.point[0]+board.point[1]<=44)cur_offset=cur_middle;
        else cur_offset=cur_ending;

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