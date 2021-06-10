#include"dekunobou.hpp"
#include"othello.hpp"
//minimaxで末端ノードのときに出力する値
#define win_out 30000//先手側が勝ったときの評価値
#define lose_out -30000//先手側が負けたときの評価値
#define draw_out 0//引き分けのときの評価値

bool turn_p;//エンジン側の手番(応急処置)

/**********paramについて************/
/**********
 * n: 序盤・中盤・終盤かを表す(n=0,1,2), eval_calcで計算している
 * 20n~20n+17: 盤面の重み
 * 20n+18: 相手の置ける場所の重みの合計に掛ける
 * 20n+19: 盤上の石のうち自分の石の占める割合に掛ける
***********/

//対称移動を考慮したパラメータと盤上のインデックスの対応表
//なんか式で書けそうな気もするけどこれが多分手っ取り早い
//最初からおいてあるところは20にする
int ref_table[64]={
     0, 1, 2, 3, 4, 5, 6, 7,
     1, 8, 9,10,11,12,13, 6,
     2, 9,14,15,16,17,12, 5,
     3,10,15,20,20,16,11, 4,
     4,11,16,20,20,15,10, 3,
     5,12,17,16,15,14, 9, 2,
     6,13,12,11,10, 9, 8, 1,
     7, 6, 5, 4, 3, 2, 1, 0
};

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board board,int move,float param[param_size]){
    int scene=std::max(0,(board.point[0]+board.point[1]-5)/20);
    float out=param[20*scene+ref_table[move]],moves_opponent_sum=0;
    //1手すすめる
    board.push(move);
    //相手の合法手をカウント
    LegalMoveList moves(board);
    for(int i=0;i<moves.size();++i)moves_opponent_sum+=param[20*scene+ref_table[moves[i]]];
    out+=param[20*scene+18]*moves_opponent_sum;
    float point_ratio=(float)board.point[!board.turn]/(board.point[!board.turn]+board.point[board.turn]);
    out+=param[20*scene+19]*point_ratio;//すでに置かれている石のうちの自分の石の割合

    //エンジン側の手番じゃなければ符号を反転させる
    //if(board.turn==turn_p)out*=-1.0;
    return out;
}

//minimax法による先読み
float minimax(Board board,float param[param_size],int depth){
    //候補手の展開
    LegalMoveList moves(board);
    if(moves.size()==0){
        //手番を変えて展開
        board.push(-1);
        LegalMoveList moves2(board);
        //終局
        if(moves2.size()==0){
            if(!turn_p){
                //エンジン側が先手のとき
                if(board.point[0]>board.point[1])return win_out;
                else if(board.point[0]<board.point[1])return lose_out;
                else return draw_out;
            }else{
                //エンジン側が後手のとき
                if(board.point[0]>board.point[1])return lose_out;
                else if(board.point[0]<board.point[1])return win_out;
                else return draw_out;
            }
        }
        moves=moves2;
    }

    float eval;
    if(board.turn==turn_p)eval=-inf;//エンジン側が手番のときは評価値の最大値を求める
    else eval=inf;//相手が手番のときは評価値の最小値を求める
    //末端ノードのとき
    if(depth<=0){
        for(int i=0;i<moves.size();++i){
            if(board.turn==turn_p){
                eval=std::max(eval,eval_calc(board,moves[i],param));
            }else{
                eval=std::min(eval,eval_calc(board,moves[i],param));
            }
        }
        //std::cout<<"depth:"<<depth<<", eval:"<<eval<<std::endl;
        return eval;
    }

    //それ以外のとき
    Board board_ref;
    for(int i=0;i<moves.size();++i){
        //1手打つ
        board_ref=board;
        board_ref.push(moves[i]);
        if(board.turn==turn_p){
            eval=std::max(eval,minimax(board_ref,param,depth-1));
        }else{
            eval=std::min(eval,minimax(board_ref,param,depth-1));
        }
    }
    //std::cout<<"depth:"<<depth<<", eval:"<<eval<<std::endl;
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
    Board board_ref;
    for(int i=0;i<moves.size();i++){
        eval_ref=eval_calc(board,moves[i],param);

        //先読みしてみる
        //board_ref=board;
        //board_ref.push(moves[i]);
        //eval_ref=minimax(board_ref,param,4);
        //std::cout<<moves[i]<<": "<<eval_ref<<std::endl;
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