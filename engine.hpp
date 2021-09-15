#include"dekunobou.hpp"
#include"othello.hpp"
std::random_device rnd_select;
bool turn_p;//エンジン側の手番

/**********paramについて************/
/*
これを参考にする
https://qiita.com/na-o-ys/items/10d894635c2a6c07ac70
盤の石の配置を10桁の3進数とみなしてこれを10進数に変換し, 対応するインデックスから値を引っ張ってくる
a*盤上における自石の割合+石の配置の評価値の合計
いずれのパラメータも序盤・中盤・終盤で変える
石の配置は3**10-1通り(何も置かれていない状況は考えなくていいので)
mapで管理
後手の石: 2
なにもない: 0
先手の石: 1
後手番で評価する際は符号を反転させる

例
石の配置: *ox
1*1+3*2+9*0=7

0~3**10-1: 石の配置
3**10: 盤上にしめる自石の割合(序盤)
3**10+1: 盤上にしめる自石の割合(中盤)
3**10+2: 盤上にしめる自石の割合(終盤)
*/

std::map<int,std::map<int,int>>shape_value{
    //先手番
    {0,{
        {-1,2},
        {0,0},
        {1,1},
    }},
    //後手番
    {1,{
        {-1,2},
        {0,0},
        {1,1},
    }}
};

//角周辺の形を計算するときに使用
//先頭が1の位
int shape_ref[8][12]={
    {0,1,2,3,9,10,11,18,19,27},
    {0,8,16,24,9,17,25,18,26,27},
    {56,48,40,32,49,41,33,42,34,35},
    {56,57,58,59,49,50,51,42,43,35},
    {63,62,61,60,54,53,52,45,44,36},
    {63,55,47,39,54,46,38,45,37,36},
    {7,15,23,31,14,22,30,21,29,28},
    {7,6,5,4,14,13,12,21,20,28}
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

//3の累乗
int pow3[10]={1,3,9,27,81,243,729,2187,6561,19683};

//角付近の形を評価する
float calc_shape_value(Board& board,float param[param_size]){
    float val=0;
    int index;
    int ref;
    //盤の石の並び
    for(int i=0;i<8;++i){
        //3進数から10進数へ変換
        index=0;
        for(int j=0;j<10;++j){
            ref=shape_ref[i][j];
            index+=pow3[j]*shape_value[turn_p][board.board[board_x[ref]][board_y[ref]]];
        }
        //val+=(param[index]-27)*0.02-1.0;
        val+=param[index];
    }
    if(!board.turn)val*=-1.0;
    return val;
}

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board board,float param[param_size]){
    //float ans=param[cur_offset]*8*board.point[!board.turn]/(board.point[0]+board.point[1]);
    float ans=calc_shape_value(board,param);//石の配置
    ans+=param[cur_offset]*8*board.point[!board.turn]/(board.point[0]+board.point[1]);
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