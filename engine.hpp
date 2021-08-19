#include"dekunobou.hpp"
#include"othello.hpp"
bool turn_p;//エンジン側の手番(応急処置)
ll nodes;//探索ノード数カウント用

/**********paramについて************/
/**********
 * n: 序盤・中盤・終盤かを表す(n=0,1,2), eval_calcで計算している
 * 11n~11n+9: 石の配置の重み(先手番目線になるので後手番のときは正負を反転させる)
 * 11n+10: 盤上における自分の石の枚数の割合の重み
***********/

//新しい評価関数の仕様
/*
これを参考にする
https://qiita.com/na-o-ys/items/10d894635c2a6c07ac70
角周辺の石の配置は3桁の3進数とみなしてこれを10進数に変換し, 対応するインデックスから値を引っ張ってくる
a*置ける場所の差分(相手-自分)+b*辺の形の評価値の合計
a, bだけ序盤・中盤・終盤で変える
mapで管理
なにもない: 0
先手の石: 1
後手の石: 2

これで先手目線にして後手番で評価するときには正負を反転させる

例
石の配置: *ox
1*0+3*1+9*2=21
0~80: 石の形の評価値(序盤)
81: a(序盤)
82~162: 石の形の評価値(中盤)
163: a(中盤)
164~244: 石の形の評価値(終盤)
245: a(終盤)
*/

std::map<int,std::map<int,int>>shape_value{
    {0,{
        {-1,2},
        {0,0},
        {1,1},
    }},
    {1,{
        {-1,2},
        {0,0},
        {1,1},
    }}
};

//角周辺の形を計算するときに使用
int shape_ref[12][4]={
    {0,1,2,3},
    {0,8,16,24},
    {7,6,5,4},
    {7,15,22,29},
    {56,48,40,32},
    {56,57,58,59},
    {63,62,61,60},
    {63,55,47,39},
    {0,9,18,27},//斜め
    {7,14,21,28},//斜め
    {56,49,42,35},//斜め
    {63,54,45,36}//斜め
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
float calc_shape_value(Board& board,float param[param_size],int cur_offset){
    float val=0;
    int index;
    int ref1,ref2,ref3,ref4;
    //角付近の形
    for(int i=0;i<12;++i){
        ref1=shape_ref[i][0];ref2=shape_ref[i][1];ref3=shape_ref[i][2];ref4=shape_ref[i][3];
        index=shape_value[turn_p][board.board[board_x[ref1]][board_y[ref1]]];
        index+=3*shape_value[turn_p][board.board[board_x[ref2]][board_y[ref2]]];
        index+=9*shape_value[turn_p][board.board[board_x[ref3]][board_y[ref3]]];
        index+=27*shape_value[turn_p][board.board[board_x[ref4]][board_y[ref4]]];
        val+=param[index+cur_offset];
    }
    //後手番のときは符号を反転
    if(!board.turn)val*=-1.0;
    return val;
}

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board& board,float param[param_size]){
    float ans=board.point[!board.turn]/(board.point[0]+board.point[1])*12;
    int cur_offset=(board.point[0]+board.point[1]-4)/20*82;
    ans*=param[cur_offset+81];
    ans+=calc_shape_value(board,param,cur_offset);
    return ans;
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
            //最終的な石の枚数を出力する
            return board.point[turn_p];
        }
        return minimax(board,param,depth-1);
        //moves=moves2;
    }

    float eval;
    if(board.turn==turn_p)eval=-inf;//エンジン側が手番のときは評価値の最大値を求める
    else eval=inf;//相手が手番のときは評価値の最小値を求める
    //末端ノードのとき
    if(depth<=0){
        for(int i=0;i<moves.size();++i){
            if(board.turn==turn_p){
                eval=std::max(eval,eval_calc(board,param));
            }else{
                eval=std::min(eval,eval_calc(board,param));
            }
        }
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
    return eval;
}

//αβ法による先読み
//α: 評価値の最小値
//β: 評価値の最大値
float alphabeta(Board board,float param[param_size],int depth,float alpha,float beta){
    //候補手の展開
    LegalMoveList moves(board);
    if(moves.size()==0){
        //手番を変えて展開
        board.push(-1);
        LegalMoveList moves2(board);
        //終局
        if(moves2.size()==0){
            ++nodes;
            return board.point[turn_p];
        }
        return alphabeta(board,param,depth-1,alpha,beta);
        moves=moves2;
    }

    float eval;
    if(board.turn==turn_p)eval=-inf;//エンジン側が手番のときは評価値の最大値を求める
    else eval=inf;//相手が手番のときは評価値の最小値を求める
    //末端ノードのとき
    Board board_ref;
    if(depth<=0){
        for(int i=0;i<moves.size();++i){
            ++nodes;
            board_ref=board;
            board_ref.push(moves[i]);
            if(board.turn==turn_p){
                eval=std::max(eval,eval_calc(board,param));
                if(eval>=beta)break;
                alpha=std::max(alpha,eval);
            }else{
                eval=std::min(eval,eval_calc(board,param));
                if(eval<=alpha)break;
                beta=std::min(beta,eval);
            }
        }
        return eval;
    }
    
    for(int i=0;i<moves.size();++i){
        //1手打つ
        board_ref=board;
        board_ref.push(moves[i]);
        if(board.turn==turn_p){
            eval=std::max(eval,alphabeta(board_ref,param,depth-1,alpha,beta));
            if(eval>=beta)break;
            alpha=std::max(alpha,eval);
        }else{
            eval=std::min(eval,alphabeta(board_ref,param,depth-1,alpha,beta));
            if(eval<=alpha)break;
            beta=std::min(beta,eval);
        }
    }
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

    //探索の優先順位付け
    float evals[64];
    int priority[64];
    bool selected[64];
    std::vector<float>evals_sort(moves.size());

    //5手読みの評価値を算出
    for(int i=0; i<moves.size();++i){
        board_ref=board;
        board_ref.push(moves[i]);
        //evals[i]=eval_calc(board,moves[i],param);
        evals[i]=alphabeta(board_ref,param,4,eval,inf);
        evals_sort[i]=evals[i];
        eval=std::max(evals[i],eval);
        selected[i]=false;
    }

    //評価値の降順にソート
    std::sort(evals_sort.begin(),evals_sort.end(),std::greater<float>());
    
    for(int i=0;i<moves.size();++i){
        for(int j=0;j<moves.size();++j){
            if(!selected[j]&&evals_sort[i]==evals[j]){
                priority[i]=j;
                selected[j]=true;
                break;
            }
        }
    }

    //for debug
    //std::cout<<"priority: ";
    //for(int i=0;i<moves.size();++i)std::cout<<priority[i]+1<<" ";
    //std::cout<<std::endl;

    //終盤完全読みのしきい値. 盤上の石の数がこれ以上になったら完全読みをする
    int qsearch_thresh=49;
    eval=-inf;
    for(int i=0;i<moves.size();i++){
        //先読みしてみる
        //1手読みしたいなら深さを0に指定する
        board_ref=board;
        board_ref.push(moves[priority[i]]);
        
        nodes=0;

        if(board.point[0]+board.point[1]>=qsearch_thresh)eval_ref=alphabeta(board_ref,param,60,-inf,inf);
        else eval_ref=alphabeta(board_ref,param,6,eval,inf);

        //for debug
        //std::cout<<priority[i]+1<<": "<<eval_ref<<" "<<nodes/1000<<"k"<<std::endl;

        if(eval_ref>eval){
            bestmoves_num=0;
            BestMoves[bestmoves_num]=moves[priority[i]];
            ++bestmoves_num;
            eval=eval_ref;
        }else if(eval_ref==eval){
            BestMoves[bestmoves_num]=moves[priority[i]];
            ++bestmoves_num;
        }
    }
    //for debug
    //std::cout<<"eval: "<<eval<<std::endl;
    return BestMoves[rnd_select()%bestmoves_num];
}