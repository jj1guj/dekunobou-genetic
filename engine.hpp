#include"dekunobou.hpp"
#include"othello.hpp"
//minimaxで末端ノードのときに出力する値
ll nodes;
bool turn_p;//エンジン側の手番(応急処置)

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board board){
    //盤上の自石の割合を返す
    //return (float)board.point[turn_p]/(board.point[0]+board.point[1]);
    return board.point[turn_p];
}

//minimax法による先読み
float minimax(Board board,int depth){
    //候補手の展開
    LegalMoveList moves(board);
    if(moves.size()==0){
        //手番を変えて展開
        board.push(-1);
        LegalMoveList moves2(board);
        //終局
        if(moves2.size()==0){
            ++nodes;
            return eval_calc(board);
        }
        //パスだったら1手深く読んでみる(相手側の評価値を計算しないように知るため)
        return minimax(board, depth-1);
        //moves=moves2;
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
                eval=std::max(eval,eval_calc(board_ref));
            }else{
                eval=std::min(eval,eval_calc(board_ref));
            }
        }
        return eval;
    }

    //それ以外のとき
    for(int i=0;i<moves.size();++i){
        //1手打つ
        board_ref=board;
        board_ref.push(moves[i]);
        if(board.turn==turn_p){
            eval=std::max(eval,minimax(board_ref,depth-1));
        }else{
            eval=std::min(eval,minimax(board_ref,depth-1));
        }
    }
    return eval;
}

//αβ法による先読み
//α: 評価値の最小値
//β: 評価値の最大値
float alphabeta(Board board,int depth,float alpha,float beta){
    //候補手の展開
    LegalMoveList moves(board);
    if(moves.size()==0){
        //手番を変えて展開
        board.push(-1);
        LegalMoveList moves2(board);
        //終局
        if(moves2.size()==0){
            ++nodes;
            return eval_calc(board);
        }
        return alphabeta(board,depth-1,alpha,beta);
        //moves=moves2;
    }

    float eval;
    if(board.turn==turn_p)eval=-inf;//エンジン側が手番のときは評価値の最大値を求める
    else eval=inf;//相手が手番のときは評価値の最小値を求める
    Board board_ref;

    //末端ノードのとき
    if(depth<=0){
        for(int i=0;i<moves.size();++i){
            ++nodes;
            board_ref=board;
            board_ref.push(moves[i]);
            if(board.turn==turn_p){
                eval=std::max(eval,eval_calc(board_ref));
                if(eval>beta)break;
                alpha=std::max(alpha,eval);
            }else{
                eval=std::min(eval,eval_calc(board_ref));
                if(eval<alpha)break;
                beta=std::min(beta,eval);
            }
        }
        return eval;
    }
    
    float eval_ref;
    for(int i=0;i<moves.size();++i){
        //1手打つ
        board_ref=board;
        board_ref.push(moves[i]);
        eval_ref=alphabeta(board_ref,depth-1,alpha,beta);
        if(board.turn==turn_p){
            eval=std::max(eval,eval_ref);
            if(eval>beta)break;
            alpha=std::max(alpha,eval);
        }else{
            eval=std::min(eval,eval_ref);
            if(eval<alpha)break;
            beta=std::min(beta,eval);
        }
    }
    return eval;
}

int go(Board board){
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
    float eval_alphabeta;
    if(board.point[0]+board.point[1]>=48){
        std::cout<<"depth: "<<63-board.point[0]-board.point[1]<<std::endl;
    }

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
        evals[i]=alphabeta(board_ref,4,eval,inf);
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

    eval=-inf;
    for(int i=0;i<moves.size();i++){
        board_ref=board;
        board_ref.push(moves[priority[i]]);

        //先読みしてみる
        nodes=0;
        if(board.point[0]+board.point[1]>=48){
            //残り20手で完全読み
            eval_ref=alphabeta(board_ref,100,-inf,inf);
        }else{
            eval_ref=alphabeta(board_ref,8,eval,inf);
        }
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
    //std::cout<<"eval: "<<eval<<std::endl;
    return BestMoves[rnd_select()%bestmoves_num];
}