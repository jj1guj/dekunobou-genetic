#include"dekunobou.hpp"
#include"othello.hpp"
//minimaxで末端ノードのときに出力する値
#define win_out 30000//先手側が勝ったときの評価値
#define lose_out -30000//先手側が負けたときの評価値
#define draw_out 0//引き分けのときの評価値

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
    out+=param[64]*moves_opponent_sum;
    //out+=param[65]*(board.point[!board.turn]-board.point[board.turn]);//自分と相手の石の数の差
    out+=param[65]*(float)board.point[!board.turn]/(board.point[!board.turn]+board.point[board.turn]);//すでに置かれている石のうちの自分の石の割合
    return out;
}

//評価値を出力する
float minimax(Board board,bool p_turn,int move,float param[param_size],int depth){
    std::cout<<depth<<": "<<board.point[0]<<" "<<board.point[1]<<" "<<board.turn<<"\n";
    //disp(board);
    if(depth<=0)return eval_calc(board,move,param);

    //1手すすめる
    board.push(move);

    //ノードの展開
    LegalMoveList moves(board);
    if(moves.size()==0){
        board.push(-1);
        LegalMoveList moves2(board);

        //末端ノードまできた
        if(moves2.size()==0){
            //disp(board);
            if(board.point[0]>board.point[1]){
                //エンジン側が後手番
                if(p_turn)return lose_out;
                return win_out;
            }
            if(board.point[0]<board.point[1]){
                //エンジン側が後手番
                if(p_turn)return win_out;
                return lose_out;
            }else return draw_out;
        }
        moves=moves2;
    }

    //各候補手に対し評価値を算出
    float eval_ref;
    int BestMoves[64];
    int bestmoves_num;
    float eval=-inf;
    for(int i=0; i<moves.size();++i){
        eval_ref=eval_calc(board,moves[i],param);
        std::cout<<moves[i]<<", "<<eval_ref<<std::endl;
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
    std::cout<<"\n";

    //最善の候補手に対し1手深く探索する
    eval=inf;
    for(int i=0;i<bestmoves_num;++i)eval=std::min(eval,minimax(board,p_turn,BestMoves[i],param,depth-1));
    return eval;
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
        //eval_ref=eval_calc(board,moves[i],param);

        //先読みしてみる
        eval_ref=minimax(board,board.turn,moves[i],param,40);
        std::cout<<i<<","<<moves[i]<<": "<<eval_ref<<std::endl;

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
    std::cout<<"eval: "<<eval<<std::endl;
    return BestMoves[rnd_select()%bestmoves_num];
}