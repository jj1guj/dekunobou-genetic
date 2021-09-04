//これを従来の盤面クラスに合わせるようにして実装
//https://qiita.com/sensuikan1973/items/459b3e11d91f3cb37e43
#include<iostream>

#pragma once
unsigned long long makeLegalBoard(unsigned long long board_player,unsigned long long board_opponent){
    //左右端の番人
    unsigned long long horizontalWatcher=board_opponent&0x7e7e7e7e7e7e7e7e;

    //上下端の番人
    unsigned long long verticalWatcher=board_opponent&0x00FFFFFFFFFFFF00;

    //全辺の番人
    unsigned long long allsideWatcher=board_opponent&0x007e7e7e7e7e7e00;

    //空きマスのみにビットが立っているボード
    unsigned long long blankBoard=~(board_player|board_opponent);

    unsigned long long tmp,legalBoard;

    //8方向で合法手があるかチェックする
    //左
    tmp=horizontalWatcher&(board_player<<1);
    for(int i=0;i<5;++i){
        tmp|=horizontalWatcher&(tmp<<1);
    }
    legalBoard=blankBoard&(tmp<<1);

    //右
    tmp=horizontalWatcher&(board_player>>1);
    for(int i=0;i<5;++i){
        tmp|=horizontalWatcher&(tmp>>1);
    }
    legalBoard|=blankBoard&(tmp>>1);

    //上
    tmp=verticalWatcher&(board_player<<8);
    for(int i=0;i<5;++i){
        tmp|=verticalWatcher&(tmp<<8);
    }
    legalBoard|=blankBoard&(tmp<<8);

    //下
    tmp=verticalWatcher&(board_player>>8);
    for(int i=0;i<5;++i){
        tmp|=verticalWatcher&(tmp>>8);
    }
    legalBoard|=blankBoard&(tmp>>8);

    //右斜め上
    tmp=allsideWatcher&(board_player<<7);
    for(int i=0;i<5;++i){
        tmp|=allsideWatcher&(tmp<<7);
    }
    legalBoard|=blankBoard&(tmp<<7);

    //左斜め上
    tmp=allsideWatcher&(board_player<<9);
    for(int i=0;i<5;++i){
        tmp|=allsideWatcher&(tmp<<9);
    }
    legalBoard|=blankBoard&(tmp<<9);

    //右斜め下
    tmp=allsideWatcher&(board_player>>9);
    for(int i=0;i<5;++i){
        tmp|=allsideWatcher&(tmp>>9);
    }
    legalBoard|=blankBoard&(tmp>>9);

    //左斜め下
    tmp=allsideWatcher&(board_player>>7);
    for(int i=0;i<5;++i){
        tmp|=allsideWatcher&(tmp>>7);
    }
    legalBoard|=blankBoard&(tmp>>7);

    return legalBoard;
}

class Board{
    public:
    bool turn=false;//後手のときtrue
    int ply=1;//今何手目か
    int point[2]={2,2};
    unsigned long long board_player=0x0000000810000000;
    unsigned long long board_opponent=0x0000001008000000;
    unsigned long long board_black=0x0000000810000000;
    unsigned long long board_white=0x0000001008000000;

    //指定した座標に何色の石がおいてあるか返す
    int operator [](int i){
        i=63-i;
        if((board_black>>i)&1){
            return 1;
        }else if((board_white>>i)&1){
            return -1;
        }else{
            return 0;
        }
    }

    //座標をビットに変換
    unsigned long long idToBit(int id){
        unsigned long long mask=0x8000000000000000;
        unsigned long long x=id>>3,y=id&7;
        mask=mask>>y;
        mask=mask>>(x*8);
        return mask;
    }

    //着手できるか判定
    bool putable(unsigned long long id){
        unsigned long long legalBoard=makeLegalBoard(board_player,board_opponent);

        return (id&legalBoard)==id;
    }

    void push(int id_int){
        if(0<=id_int&&id_int<64){
            unsigned long long id=idToBit(id_int);
            unsigned long long rev=0;
            
            for(int i=0;i<8;++i){
                unsigned long long rev_=0;
                unsigned long long mask=transfer(id,i);
                
                while((mask!=0)&&((mask&board_opponent)!=0)){
                    rev_|=mask;
                    mask=transfer(mask,i);
                }

                if((mask&board_player)!=0){
                    rev|=rev_;
                }
            }

            //反転する
            board_player^=(id|rev);
            board_opponent^=rev;
        }

        //手番等の更新
        swapBoard();
        ++ply;
    }

    //パスの判定
    bool is_pass(){
        unsigned long long playerLegalBoard=makeLegalBoard(board_player,board_opponent);
        unsigned long long opponentLegalBoard=makeLegalBoard(board_opponent,board_player);

        return (playerLegalBoard == 0x0000000000000000) && (opponentLegalBoard != 0x0000000000000000);
    }

    //終局の判定
    bool is_over(){
        unsigned long long playerLegalBoard=makeLegalBoard(board_player,board_opponent);
        unsigned long long opponentLegalBoard=makeLegalBoard(board_opponent,board_player);

        return (playerLegalBoard == 0x0000000000000000) && (opponentLegalBoard == 0x0000000000000000);
    }

    //手番交代
    void swapBoard(){
        unsigned long long tmp=board_player;
        board_player=board_opponent;
        board_opponent=tmp;
        turn=!turn;

        if(turn){
            board_white=board_player;
            board_black=board_opponent;
        }else{
            board_black=board_player;
            board_white=board_opponent;
        }

        //枚数の更新
        point[0]=bitcount(board_black);
        point[1]=bitcount(board_white);
    }

    private:
    unsigned long long transfer(unsigned long long id,int dir){
        if(dir==0){
            //上
            return (id << 8) & 0xffffffffffffff00;
        }else if(dir==1){
            //右上
            return (id << 7) & 0x7f7f7f7f7f7f7f00;
        }else if(dir==2){
            //右
            return (id >> 1) & 0x7f7f7f7f7f7f7f7f;
        }else if(dir==3){
            //右下
            return (id >> 9) & 0x007f7f7f7f7f7f7f;
        }else if(dir==4){
            //下
            return (id >> 8) & 0x00ffffffffffffff;
        }else if(dir==5){
            //左下
            return (id >> 7) & 0x00fefefefefefefe;
        }else if(dir==6){
            //左
            return (id << 1) & 0xfefefefefefefefe;
        }else if(dir==7){
            //左上
            return (id << 9) & 0xfefefefefefefe00;
        }
        return 0;
    }

    int bitcount(unsigned long long data){
        data=(data&0x5555555555555555)+((data&0xaaaaaaaaaaaaaaaa)>>1);//2桁ごとの1の数
        data=(data&0x3333333333333333)+((data&0xcccccccccccccccc)>>2);//4桁ごとの1の数
        data=(data&0xf0f0f0f0f0f0f0f)+((data&0xf0f0f0f0f0f0f0f0)>>4);//8桁ごとの1の数
        data=(data&0xff00ff00ff00ff)+((data&0xff00ff00ff00ff00)>>8);//16桁ごとの1の数
        data=(data&0xffff0000ffff)+((data&0xffff0000ffff0000)>>16);//32桁ごとの1の数
        data=(data&0xffffffff)+((data&0xffffffff00000000)>>32);//64桁の1の数
        return data;
    }
};

class LegalMoveList{
    public:
    LegalMoveList(){}
    LegalMoveList(Board board){
        unsigned long long legalBoard=makeLegalBoard(board.board_player,board.board_opponent);
        move_num=0;
        unsigned long long id;
        for(int i=0;i<64;++i){
            id=board.idToBit(i);
            if((id&legalBoard)==id){
                movelist[move_num]=i;
                ++move_num;
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

//盤面の表示
void disp(Board board){
    for(int i=0;i<8;++i){
        for(int j=0;j<8;++j){
            int id=63-(8*i+j);
            if((board.board_black>>id)&1){
                std::cout<<"o ";
            }else if((board.board_white>>id)&1){
                std::cout<<"x ";
            }else{
                std::cout<<". ";
            }
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

//盤面の表示(合法手表示含む)
void disp_teban(Board board){
    LegalMoveList moves(board);
    int cur=0;
    for(int i=0;i<8;++i){
        for(int j=0;j<8;++j){
            int id=63-(8*i+j);
            if((board.board_black>>id)&1){
                std::cout<<"o ";
            }else if((board.board_white>>id)&1){
                std::cout<<"x ";
            }else if(moves[cur]==8*i+j && cur<moves.size()){
                ++cur;
                std::cout<<cur;
                if(cur<10)std::cout<<" ";
            }else{
                std::cout<<". ";
            }
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}