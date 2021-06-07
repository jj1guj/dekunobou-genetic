#include"dekunobou.hpp"
#include"othello.hpp"
//minimaxで末端ノードのときに出力する値
#define win_out 30000//先手側が勝ったときの評価値
#define lose_out -30000//先手側が負けたときの評価値
#define draw_out 0//引き分けのときの評価値

bool turn_p;//エンジン側の手番(応急処置)

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
    float point_ratio=(float)board.point[!board.turn]/(board.point[!board.turn]+board.point[board.turn]);
    //point_ratio=2*point_ratio-1;//-1~1の範囲を取るようにする
    out+=param[65]*point_ratio;//すでに置かれている石のうちの自分の石の割合

    //エンジン側の手番じゃなければ符号を反転させる
    if(board.turn==turn_p)out*=-1.0;
    return out;
}

//minimax法による先読み
//末端ノード以外は探索前に1手進めてるので自分の手番のつもりで呼び出すと最初は相手の手番になっている
float minimax(Board board,int move,float param[param_size],int depth){
    //末端ノードまできた
    if(depth<=0){
        std::cout<<"depth "<<depth<<", eval: "<<eval_calc(board,move,param)<<std::endl;
        return eval_calc(board,move,param);
    }

    //1手すすめる
    board.push(move);

    //ノードの展開
    LegalMoveList moves(board);
    if(moves.size()==0){
        board.push(-1);
        LegalMoveList moves2(board);

        //終局しちゃった
        if(moves2.size()==0){
            //disp(board);
            if(board.point[0]>board.point[1]){
                //エンジン側が後手番
                if(turn_p)return lose_out;
                return win_out;
            }
            if(board.point[0]<board.point[1]){
                //エンジン側が後手番
                if(turn_p)return win_out;
                return lose_out;
            }else return draw_out;
        }
        moves=moves2;
    }

    //各候補手に対し評価値を算出
    float eval_ref;
    int BestMoves[64];
    int bestmoves_num;
    float eval;
    Board board_ref;
    if(board.turn==turn_p)eval=-inf;//相手の手番のときは評価値の最小値を求める
    else eval=inf;//自分の手番のときは評価値の最大値を求める

    //各候補手について末端ノードの評価値を見ていく
    for(int i=0; i<moves.size();++i){
        if(board.turn==turn_p){
            //相手の手番のときは評価値の最小値を求める
            eval=std::max(eval,minimax(board,moves[i],param,depth-1));
        }else{
            //自分の手番のときは評価値の最大値を求める
            eval=std::min(eval,minimax(board,moves[i],param,depth-1));
        }
    }
    std::cout<<"depth "<<depth<<", eval: "<<eval<<std::endl;
    return eval;
}

int go(Board board,float param[param_size]){
    float eval=-inf;
    LegalMoveList moves(board);
    //1手だけのときはその手を返す
    if(moves.size()==1)return moves[0];

    turn_p=board.turn;//エンジン側の手番を取得

    int BestMoves[64];
    int bestmoves_num;
    std::random_device rnd_select;
    
    bestmoves_num=0;
    float eval_ref;

    //現在の評価値を算出
    for(int i=0;i<moves.size();i++){
        //eval_ref=eval_calc(board,moves[i],param);

        //先読みしてみる
        eval_ref=minimax(board,moves[i],param,2);
        std::cout<<"result "<<i<<","<<moves[i]<<": "<<eval_ref<<std::endl<<std::endl;
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