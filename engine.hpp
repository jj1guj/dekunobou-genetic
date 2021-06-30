#include"dekunobou.hpp"
#include"othello.hpp"
std::random_device rnd_select;
bool turn_p;//エンジン側の手番

/**********paramについて************/
/**********
 * n: 序盤・中盤・終盤かを表す(n=0,1,2), eval_calcで計算している
 * 20n~20n+9: 石の配置の重み(手番側だけ足すので常時手番側からみた値になる)
 * 20n+10~20n+18: 相手が置ける場所の重み
 * 20n+19: 盤上における自分の石の枚数の割合の重み
***********/
//新しい評価関数の仕様
/*
これを参考にする
https://qiita.com/na-o-ys/items/10d894635c2a6c07ac70
角周辺の石の配置は3桁の3進数とみなしてこれを10進数に変換し, 対応するインデックスから値を引っ張ってくる
a*置ける場所の差分(相手-自分)+b*辺の形の評価値の合計
a, bだけ序盤・中盤・終盤で変える
mapで管理
相手の石: 0
なにもない: 1
自分の石: 2
例
石の配置: *ox
1*1+3*2+9*0=7

0~26: 角付近の形の評価値
27~28: a,b(序盤)
29~30: a,b(中盤)
31~32: a,b(終盤)
*/

std::map<int,std::map<int,int>>shape_value{
    {0,{
        {-1,0},
        {0,1},
        {1,2},
    }},
    {1,{
        {-1,2},
        {0,1},
        {1,0},
    }}
};

//角周辺の形を計算するときに使用
int shape_ref[12][3]={
    {0,1,2},
    {0,8,16},
    {7,6,5},
    {7,15,22},
    {56,48,40},
    {56,57,58},
    {63,62,61},
    {63,55,47},
    {0,9,18},//斜め
    {7,14,21},//斜め
    {56,49,42},//斜め
    {63,54,45}//斜め
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

//角付近の形を評価する
float calc_shape_value(Board& board,float param[param_size]){
    float val=0;
    int index;
    int ref1,ref2,ref3;
    //角付近の形
    for(int i=0;i<12;++i){
        ref1=shape_ref[i][0];ref2=shape_ref[i][1];ref3=shape_ref[i][2];
        index=shape_value[turn_p][board.board[board_x[ref1]][board_y[ref1]]];
        index+=3*shape_value[turn_p][board.board[board_x[ref2]][board_y[ref2]]];
        index+=9*shape_value[turn_p][board.board[board_x[ref3]][board_y[ref3]]];
        val+=param[index];
    }
    return val;
}

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board board,float param[param_size]){
    LegalMoveList moves(board);
    float ans=moves.size();
    board.push(-1);
    LegalMoveList moves2(board);
    ans-=moves2.size();
    ans*=param[cur_offset];
    ans=param[cur_offset+1]*calc_shape_value(board,param);
    return ans;
}

int go(Board board,float param[param_size]){
    float eval=-inf;
    LegalMoveList moves(board);
    //1手だけのときはその手を返す
    if(moves.size()==1)return moves[0];

    int BestMoves[64];
    int bestmoves_num;
    
    bestmoves_num=0;
    float eval_ref;

    //現在の評価値を算出
    Board board_ref;
    turn_p=board.turn;
    for(int i=0;i<moves.size();i++){
        board_ref=board;
        board_ref.push(moves[i]);
        eval_ref=eval_calc(board_ref,param);

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