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
float ddot(Board& board,int& cur_offset,float param[param_size]){
    float ans=0,div=0;
    //にゃにゃんメソッドを使ってみる
    //URL: https://twitter.com/Nyanyan_Cube/status/1407694024136265729?s=20
    for(int i=0;i<64;++i){
        ans+=board.board[board_x[i]][board_y[i]]*param[cur_offset+ref_table[i]];
        div+=std::abs(board.board[board_x[i]][board_y[i]]*param[cur_offset+ref_table[i]]);
    }
    if(turn_p)ans*=-1;
    return ans/div;
}

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board& board,float param[param_size]){
    int cur_offset=(board.point[0]+board.point[1]-4)/20;
    float ans=ddot(board,cur_offset,param);
    //石の枚数に対してもにゃにゃんメソッドを使用
    //URL: https://twitter.com/Nyanyan_Cube/status/1407694260242055172?s=20
    ans+=param[cur_offset+19]*(board.point[turn_p]-board.point[!turn_p])/(board.point[0]+board.point[1]);
    
    LegalMoveList moves(board);//相手の置ける場所
    for(int i=0;i<moves.size();++i)ans+=param[cur_offset+ref_table_move[moves[i]]];
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
    std::cout<<"priority: ";
    for(int i=0;i<moves.size();++i)std::cout<<priority[i]+1<<" ";
    std::cout<<std::endl;

    if(board.point[0]+board.point[1]>=49)std::cout<<"depth: "<<63-board.point[0]-board.point[1]<<std::endl;
    eval=-inf;
    for(int i=0;i<moves.size();i++){
        //先読みしてみる
        //1手読みしたいなら深さを0に指定する
        board_ref=board;
        board_ref.push(moves[priority[i]]);
        
        nodes=0;
        //終盤20手で完全読み
        if(board.point[0]+board.point[1]>=49)eval_ref=alphabeta(board_ref,param,60,-inf,inf);
        else eval_ref=alphabeta(board_ref,param,8,eval,inf);
        std::cout<<priority[i]+1<<": "<<eval_ref<<" "<<nodes/1000<<"k"<<std::endl;

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
    std::cout<<"eval: "<<eval<<std::endl;
    return BestMoves[rnd_select()%bestmoves_num];
}