#include"dekunobou.hpp"
#include"othello.hpp"
//minimaxで末端ノードのときに出力する値
#define win_out 30000//先手側が勝ったときの評価値
#define lose_out -30000//先手側が負けたときの評価値
#define draw_out 0//引き分けのときの評価値

bool turn_p;//エンジン側の手番(応急処置)
float a,b;

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
    float point_ratio=(float)board.point[!board.turn]/(board.point[!board.turn]+board.point[board.turn]);
    out+=param[65]*point_ratio;//すでに置かれている石のうちの自分の石の割合
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
                eval=std::max(eval,eval_calc(board,moves[i],param));
            }else{
                eval=std::min(eval,eval_calc(board,moves[i],param));
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
            if(board.turn==turn_p){
                eval=std::max(eval,eval_calc(board,moves[i],param));
                if(eval>=beta)break;
                alpha=std::max(alpha,eval);
                a=alpha;
            }else{
                eval=std::min(eval,eval_calc(board,moves[i],param));
                if(eval<=alpha)break;
                beta=std::min(beta,eval);
                b=beta;
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
            a=alpha;
        }else{
            eval=std::min(eval,alphabeta(board_ref,param,depth-1,alpha,beta));
            if(eval<=alpha)break;
            beta=std::min(beta,eval);
            b=beta;
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
    a=-inf;
    b=inf;

    //探索の優先順位付け
    //たぶん今バグってるのでやらない
    /*float evals[64];
    int priority[64];
    bool selected[64];
    std::vector<float>evals_sort(moves.size());
    //1手読みの評価値を算出
    for(int i=0; i<moves.size();++i){
        evals[i]=eval_calc(board,moves[i],param);
        evals_sort[i]=evals[i];
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
    std::cout<<std::endl;*/

    if(board.point[0]+board.point[1]>=44)std::cout<<"depth: "<<63-board.point[0]-board.point[1]<<std::endl;

    for(int i=0;i<moves.size();i++){
        //先読みしてみる
        //1手読みしたいなら深さを0に指定する
        board_ref=board;
        //board_ref.push(moves[priority[i]]);
        board_ref.push(moves[i]);
        //eval_ref=minimax(board_ref,param,6);
        
        //終盤20手で完全読み
        if(board.point[0]+board.point[1]>=44)eval_ref=alphabeta(board_ref,param,63-board.point[0]-board.point[1],-inf,inf);
        else eval_ref=alphabeta(board_ref,param,6,-inf,inf);
        //std::cout<<priority[i]+1<<": "<<eval_ref<<std::endl;
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
    std::cout<<"eval: "<<eval<<std::endl;
    return BestMoves[rnd_select()%bestmoves_num];
}