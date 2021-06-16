#include"dekunobou.hpp"
#include"othello.hpp"
//minimaxで末端ノードのときに出力する値

bool turn_p;//エンジン側の手番(応急処置)

//評価値の計算(手番側が有利ならプラス)
float eval_calc(Board board){
    //盤上の自石の割合を返す
    return (float)board.point[turn_p]/(board.point[0]+board.point[1]);
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
            return eval_calc(board);
        }
        moves=moves2;
    }

    float eval;
    if(board.turn==turn_p)eval=-inf;//エンジン側が手番のときは評価値の最大値を求める
    else eval=inf;//相手が手番のときは評価値の最小値を求める
    //末端ノードのとき
    Board board_ref;
    if(depth<=0){
        for(int i=0;i<moves.size();++i){
            board_ref=board;
            board_ref.push(moves[i]);
            if(board.turn!=turn_p){
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
float alphabeta(Board board,int depth,float alpha,float beta){
    //候補手の展開
    LegalMoveList moves(board);
    if(moves.size()==0){
        //手番を変えて展開
        board.push(-1);
        LegalMoveList moves2(board);
        //終局
        if(moves2.size()==0){
            return eval_calc(board);
        }
        moves=moves2;
    }

    float eval;
    if(board.turn==turn_p)eval=-inf;//エンジン側が手番のときは評価値の最大値を求める
    else eval=inf;//相手が手番のときは評価値の最小値を求める
    //末端ノードのとき
    Board board_ref;
    if(depth<=0){
        for(int i=0;i<moves.size();++i){
            board_ref=board;
            board_ref.push(moves[i]);
            
            if(board.turn==turn_p){
                eval=std::max(eval,eval_calc(board_ref));
            }else{
                eval=std::min(eval,eval_calc(board_ref));
            }
        }
        //std::cout<<eval<<std::endl;
        return eval;
    }

    //それ以外のとき
    for(int i=0;alpha<beta && i<moves.size();++i){
        //1手打つ
        board_ref=board;
        board_ref.push(moves[i]);
        eval=alphabeta(board_ref,depth-1,alpha,beta);

        if(board.turn==turn_p && eval<beta){
            beta=eval;//βカット
        }else if(eval>alpha){
            alpha=eval;///αカット
        }
    }
    if(board.turn==turn_p)return beta;
    else return alpha;
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
    for(int i=0;i<moves.size();i++){
        board_ref=board;
        board_ref.push(moves[i]);
        //1手読み
        //eval_ref=eval_calc(board_ref);
        //先読みしてみる
        //eval_ref=minimax(board_ref,6);
        eval_ref=alphabeta(board_ref,8,-inf,inf);
        std::cout<<i+1<<": "<<eval_ref<<std::endl;
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