#include"dekunobou.hpp"
#include"othello.hpp"

/**********paramについて************/
/**********
 0~63: 着手に対する重み
 64: 置ける場所の数に対する重み
 65: 多分確定石(近似値)に対する重み
***********/

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board board,int move,float param[param_size]){
    float out=param[move],moves_opponent_sum=0;
    //1手すすめる
    board.push(move);
    //相手の合法手をカウント
    LegalMoveList moves(board);
    for(int i=0;i<moves.size();++i)moves_opponent_sum+=param[moves[i]];
    return out+param[64]*moves_opponent_sum+param[65]*(board.point[board.turn]-board.point[!board.turn]);
}

void minimax(Board board,float param[param_size],int depth){
    float eval_max=-inf;
    int BestMoves[64];
    int bestmoves_num=0;
    //ノードの展開
    LegalMoveList moves(board);
    
}

int go(Board board,float param[param_size]){
    float eval=-inf;
    LegalMoveList moves(board);
    //1手だけのときはその手を返す
    if(moves.size()==1)return moves[0];

    int BestMoves[64];
    int bestmoves_num;
    std::random_device rnd_select;
    
    bestmoves_num=0;
    float eval_ref;

    //現在の評価値を算出
    for(int i=0;i<moves.size();i++){
        eval_ref=eval_calc(board,moves[i],param);

        if(eval_ref>eval){
            bestmoves_num=0;
            BestMoves[bestmoves_num]=moves[i];
            ++bestmoves_num;
            eval=eval_ref;
        }else if(eval_ref==eval){
            BestMoves[bestmoves_num]=moves[i];
            ++bestmoves_num;
        }
    }
    //for debug
    //std::cout<<"eval: "<<eval<<std::endl;
    return BestMoves[rnd_select()%bestmoves_num];
}