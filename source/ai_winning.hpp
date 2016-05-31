#include "4moku.hpp"
#include <ctime>

std::tuple<int,int> ai_winning(const Board& board,int player) {
    // 自分が勝つことが確定するような置き場所があるならそこに置く。
    // そうでなければランダムに置く。
    
	int nx,ny;
	std::tie(nx,ny) = board.size();

	std::mt19937 mt(std::time(0));
	std::uniform_int_distribution<> rndx(0,nx);
	std::uniform_int_distribution<> rndy(0,ny);
	
	std::tuple<int,int> random_choice = std::make_tuple(-1,-1);
	
	for(int i=0;i<300;++i) {
		int x=rndx(mt), y=rndy(mt);
		if(placeable(board, x, y)){
			// 置けるなら
			Board board_tmp(board);
			board_tmp(x,y) = player_id(player);
			
			if(finished(board_tmp)){
				// そこに置いて勝つなら
				return std::make_tuple(x,y);
			}else{
				// そうでないなら
				if(std::get<0>(random_choice) == -1){
					random_choice = std::make_tuple(x,y);
				}
			}
		}
	}
	
	return random_choice;
}
