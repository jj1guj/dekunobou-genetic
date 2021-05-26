#include"dekunobou.hpp"
#pragma once

class Board{
    public:
    int board[8][8];//先手:1, 後手:-1
    bool turn=false;//先手ならfalse, 後手ならtrue
    int point[2];//各手番の獲得枚数{先手, 後手}
    int flip_limit[8];//縦横斜め8方向どこまでひっくり返していいか
    //{横(左), 横(右), 縦(上), 縦(下), 右斜め(上), 右斜め(下), 左斜め(上), 左斜め(下)}
    Board(){init();}

    void init(){
        for(int i=0;i<64;++i)board[i/8][i%8]=0;
        board[3][3]=-1;
        board[3][4]=1;
        board[4][3]=1;
        board[4][4]=-1;
        point[0]=2;
        point[1]=2;
    }

    //パスは0~63以外の数字にする
    int push(int move){
        if(0<=move&&move<=63){
            int row=move/8,col=move%8;
            int fliped=0;//石を返した枚数
            if(board[row][col]!=0)return -1;
            board[row][col]=stone[turn];

            set_flip_limit(row,col);
            
            //石を返す
            for(int dir=0;dir<8;++dir){
                for(int i=1;i<flip_limit[dir];++i){
                    board[row+di[dir]*i][col+dj[dir]*i]=stone[turn];
                    ++fliped;
                }
            }
            if(fliped==0)return -1;

            //着手後の石の枚数を計算
            point[turn]+=fliped+1;
            point[!turn]-=fliped;
        }
        turn=!turn;//手番を反転
        return 0;
    }

    void set_flip_limit(int row,int col){
        //どこまで石を返していいかしらべる
        //コードもっと短くできる気がする
        //横左方向
        flip_limit[0]=0;
        for(int i=1;i<=col;++i){
            if(board[row][col-i]!=stone[!turn]){
                if(board[row][col-i]==stone[turn])flip_limit[0]=i;
                break;
            }
        }

        //横右方向
        flip_limit[1]=0;
        for(int i=1;i<=7-col;++i){
            if(board[row][col+i]!=stone[!turn]){
                if(board[row][col+i]==stone[turn])flip_limit[1]=i;
                break;
            }
        }

        //縦上方向
        flip_limit[2]=0;
        for(int i=1;i<=row;++i){
            if(board[row-i][col]!=stone[!turn]){
                if(board[row-i][col]==stone[turn])flip_limit[2]=i;
                break;
            }
        }

        //縦下方向
        flip_limit[3]=0;
        for(int i=1;i<=7-row;++i){
            if(board[row+i][col]!=stone[!turn]){
                if(board[row+i][col]==stone[turn])flip_limit[3]=i;
                break;
            }
        }

        //右斜め上方向
        flip_limit[4]=0;
        for(int i=1;i<=std::min(row,7-col);++i){
            if(board[row-i][col+i]!=stone[!turn]){
                if(board[row-i][col+i]==stone[turn])flip_limit[4]=i;
                break;
            }
        }

        //右斜め下方向
        flip_limit[5]=0;
        for(int i=1;i<=std::min(7-row,7-col);++i){
            if(board[row+i][col+i]!=stone[!turn]){
                if(board[row+i][col+i]==stone[turn])flip_limit[5]=i;
                break;
            }
        }

        //左斜め上方向
        flip_limit[6]=0;
        for(int i=1;i<=std::min(row,col);++i){
            if(board[row-i][col-i]!=stone[!turn]){
                if(board[row-i][col-i]==stone[turn])flip_limit[6]=i;
                break;
            }
        }

        //左斜め下方向
        flip_limit[7]=0;
        for(int i=1;i<=std::min(7-row,col);++i){
            if(board[row+i][col-i]!=stone[!turn]){
                if(board[row+i][col-i]==stone[turn])flip_limit[7]=i;
                break;
            }
        }
    }

    private:
    int stone[2]={1,-1};
    //{横(左), 横(右), 縦(上), 縦(下), 右斜め(上), 右斜め(下), 左斜め(上), 左斜め(下)}
    int di[8]={0,0,-1,1,-1,1,-1,1};
    int dj[8]={-1,1,0,0,1,1,-1,-1};
};

class LegalMoveList{
    public:
    LegalMoveList(){}
    LegalMoveList(Board board){
        move_num=0;
        for(int i=0;i<8;++i)for(int j=0;j<8;++j){
            if(board.board[i][j]==0){
                board.set_flip_limit(i,j);
                for(int k=0;k<8;++k){
                    if(board.flip_limit[k]>1){
                        movelist[move_num]=8*i+j;
                        ++move_num;
                        break;
                    }
                }
            }
        }
    }

    int size(){return move_num;}

    //添字で合法手を取得
    int operator [](int i){
        return this->movelist[i];
    }

    private:
    int movelist[64];
    int move_num;

};

void disp(Board board){
    for(int i=0;i<8;++i){
        for(int j=0;j<8;++j){
            if(board.board[i][j]==0){
                std::cout<<"* ";
            }else if(board.board[i][j]==1){
                std::cout<<"o ";
            }else{
                std::cout<<"x ";
            }
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

void disp_teban(Board board){
    LegalMoveList moves(board);
    int moves_count=0;
    for(int i=0;i<8;++i){
        for(int j=0;j<8;++j){
            //合法手だったらidを表示
            if(moves.size()>0 && moves[moves_count]==8*i+j){
                ++moves_count;
                if(moves_count>=10)std::cout<<moves_count;
                else std::cout<<moves_count<<" ";
            }else if(board.board[i][j]==0)std::cout<<"* ";
            else if(board.board[i][j]==1)std::cout<<"o ";
            else if(board.board[i][j]==-1)std::cout<<"x ";
        }
        std::cout<<std::endl;
    }
}