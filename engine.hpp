#include"dekunobou.hpp"
#include"othello.hpp"

bool turn_p;//エンジン側の手番(応急処置)

/**********paramについて************/
/**********
 * n: 序盤・中盤・終盤かを表す(n=0,1,2), eval_calcで計算している
 * 20n~20n+8: 盤面の重み
 * 20n+9: 相手の置ける場所の重みの合計に掛ける
 * 20n+10~20n+18: 石の配置(これと盤で内積を取る. 後手番なら反転)
 * 20n+19: 盤上の石のうち自分の石の占める割合に掛ける
***********/

//対称移動を考慮したパラメータと盤上のインデックスの対応表
//なんか式で書けそうな気もするけどこれが多分手っ取り早い
//最初からおいてあるところは9にする
int ref_table[64]={
     0,1,2,3,3,2,1,0,
     1,4,5,6,6,5,4,1,
     2,5,7,8,8,7,5,2,
     3,6,8,9,9,8,6,3,
     3,6,8,9,9,8,6,3,
     2,5,7,8,8,7,5,2,
     1,4,5,6,6,5,4,1,
     0,1,2,3,3,2,1,0,
};

//8で割った商(盤の行に対応)
int board_x[64]={
    0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,
};

//8で割った余り(盤の列に対応)
int board_y[64]={
    0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,
};

float ddot(Board& board,float param[param_size]){
    float ans=0;
    for(int i=0;i<64;++i)ans+=board.board[board_x[i]][board_y[i]]*param[cur_offset+10+ref_table[i]];
    if(board.turn)ans*=-1;
    return ans;
}

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board board,int move,float param[param_size]){
    //自分の打つ場所の重み
    float out=param[cur_offset+ref_table[move]],moves_opponent_sum=0;
    //1手すすめる
    board.push(move);
    //相手の合法手をカウント
    LegalMoveList moves(board);
    for(int i=0;i<moves.size();++i)moves_opponent_sum+=param[cur_offset+ref_table[moves[i]]];
    out+=param[cur_offset+9]*moves_opponent_sum;

    //石の配置
    out+=ddot(board,param);

    float point_ratio=(float)board.point[!board.turn]/(board.point[!board.turn]+board.point[board.turn]);
    out+=param[cur_offset+19]*point_ratio;//すでに置かれている石のうちの自分の石の割合
    return out;
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