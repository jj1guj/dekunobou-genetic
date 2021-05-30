#include"dekunobou.hpp"
#include"othello.hpp"

/**********paramについて************/
/**********
 0~63: 着手に対する重み
 64: 置ける場所の数に対する重み
 65: 多分確定石(近似値)に対する重み
***********/
int BestMoves[64];
int bestmoves_num;
std::random_device rnd_select;

//評価値の計算(手番側が有利ならプラス)
float calc_eval(Board board,float param[param_size]){
    float out=0;
    for(int i=0;i<8;++i)for(int j=0;j<8;++j){
        out+=board.board[i][j]*param[8*i+j];
    }
    if(board.turn)out*=-1.0;
    return param[64]*out;
}

int go(Board board,float param[param_size]){
    float eval=-std::numeric_limits<float>::infinity();
    LegalMoveList moves(board);
    //1手だけのときはその手を返す
    if(moves.size()==1)return moves[0];
    
    bestmoves_num=0;
    Board ref;
    float eval_ref,opponent_ref;

    //現在の評価値を算出
    //float eval_now=calc_eval(board,param);
    //float eval_diff;
    for(int i=0;i<moves.size();i++){
        ref=board;

        //石を返す
        ref.push(moves[i]);

        //相手の打てる場所を数える
        LegalMoveList moves_opponent(ref);
        opponent_ref=0;
        //相手がどんなところに置けるか
        for(int j=0;j<moves_opponent.size();++j)opponent_ref+=param[moves_opponent[j]];
        //評価値の算出
        eval_ref=param[moves[i]]+param[64]*opponent_ref;

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
    return BestMoves[rnd_select()%bestmoves_num];
}