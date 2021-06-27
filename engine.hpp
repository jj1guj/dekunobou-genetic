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

//対称移動を考慮したパラメータと盤上のインデックスの対応表
//なんか式で書けそうな気もするけどこれが多分手っ取り早い
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

//置ける場所の重みのパラメータと盤上のインデックスの対応表
int ref_table_move[64]={
     10,11,12,13,13,12,11,10,
     11,14,15,16,16,15,14,11,
     12,15,17,18,18,17,15,12,
     13,16,18,19,19,18,16,13,
     13,16,18,19,19,18,16,13,
     12,15,17,18,18,17,15,12,
     11,14,15,16,16,15,14,11,
     10,11,12,13,13,12,11,10,
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

//石の配置の評価
//常に先手側がいいと+になるので評価関数の仕様上後手番のときは符号を反転させる
float ddot(Board& board,float param[param_size]){
    float ans=0,div=0;
    //にゃにゃんメソッドを使ってみる
    //URL: https://twitter.com/Nyanyan_Cube/status/1407694024136265729?s=20
    if(!turn_p){
        //先手番
        for(int i=0;i<64;++i){
            if(board.board[board_x[i]][board_y[i]]==1)ans+=param[cur_offset+ref_table[i]];
            div+=std::abs(board.board[board_x[i]][board_y[i]]*param[cur_offset+ref_table[i]]);    
        }
    }else{
        //後手番
        for(int i=0;i<64;++i){
            if(board.board[board_x[i]][board_y[i]]==-1)ans+=param[cur_offset+ref_table[i]];
            div+=std::abs(board.board[board_x[i]][board_y[i]]*param[cur_offset+ref_table[i]]);    
        }
    }
    /*for(int i=0;i<64;++i){
        ans+=board.board[board_x[i]][board_y[i]]*param[cur_offset+ref_table[i]];
        div+=std::abs(board.board[board_x[i]][board_y[i]]*param[cur_offset+ref_table[i]]);
    }
    if(turn_p)ans*=-1;*/
    return ans/div;
}

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board& board,float param[param_size]){
    float ans=ddot(board,param);
    //石の枚数に対してもにゃにゃんメソッドを使用
    //URL: https://twitter.com/Nyanyan_Cube/status/1407694260242055172?s=20
    //ans+=param[cur_offset+10]*(board.point[turn_p]-board.point[!turn_p])/(board.point[0]+board.point[1]);
    LegalMoveList moves(board);//相手の置ける場所
    for(int i=0;i<moves.size();++i)ans+=param[cur_offset+ref_table_move[moves[i]]];
    ans+=param[cur_offset+19]*board.point[turn_p]/(board.point[0]+board.point[1]);
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
        eval_ref=eval_calc(board,param);

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