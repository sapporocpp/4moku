#include "4moku.hpp"
std::tuple<int,int> AI_FUNCTION(const Board& board,int player) {
    int nx,ny;
    std::tie(nx,ny) = board.size();
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> rndx(0,nx);
    std::uniform_int_distribution<> rndy(0,ny);

    for(int x=0;x<nx;++x) {
        for(int y=0;y<ny;++y) {
            /* 2つ連続する自分の石を探す */
            if(player_id(player)==board(x,y)
                && player_id(player)==board(x+1,y)) {
                if(placeable(board,x+2,y)) {
                    return std::make_tuple(x+2,y);
                }
                if(placeable(board,x-1,y)) {
                    return std::make_tuple(x-1,y);
                }
            }
            if(player_id(player)==board(x,y)
                && player_id(player)==board(x,y-1)) {
                if(placeable(board,x,y-2)) {
                    return std::make_tuple(x,y-2);
                }
                if(placeable(board,x,y+1)) {
                    return std::make_tuple(x,y+1);
                }
            }
            if(player_id(player)==board(x,y)
                && player_id(player)==board(x+1,y-1)) {
                if(placeable(board,x+2,y-2)) {
                    return std::make_tuple(x+2,y-2);
                }
                if(placeable(board,x-1,y+1)) {
                    return std::make_tuple(x-1,y+1);
                }
            }
            if(player_id(player)==board(x,y)
                && player_id(player)==board(x+1,y+1)) {
                if(placeable(board,x+2,y+2)) {
                    return std::make_tuple(x+2,y+2);
                }
                if(placeable(board,x-1,y-1)) {
                    return std::make_tuple(x-1,y-1);
                }
            }
        }
    }

    for(int x=0;x<nx;++x) {
        for(int y=0;y<ny;++y) {
            /* 自分の石を探す */
            if(player_id(player)==board(x,y)) {
                /* 周囲におけるかチェック */
                for(int ix=x-1;ix<x+2;++ix) {
                    for(int iy=y-1;iy<y+2;++iy) {
                        if(placeable(board,ix,iy)) {
                            return std::make_pair(ix,iy);
                        }
                    }
                }
            }
        }
    }

    std::tuple<int,int> random_choice = std::make_tuple(-1,-1);

    for(int i=0;i<300;++i) {
        int x=rndx(mt), y=rndy(mt);
        if(placeable(board, x, y)){
            /* 置けるなら */
            Board board_tmp(board);
            board_tmp(x,y) = player_id(player);

            if(finished(board_tmp)){
                /* そこに置いて勝つなら */
                return std::make_tuple(x,y);
            }else{
                /* そうでないなら */
                if(std::get<0>(random_choice) == -1){
                    random_choice = std::make_tuple(x,y);
                }
            }
        }
    }
    return random_choice;
}
