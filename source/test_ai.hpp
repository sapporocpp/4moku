#include "4moku.hpp"

std::tuple<int,int> AI_FUNCTION(const Board& board,int /*player*/) {
	int nx,ny;
	std::tie(nx,ny) = board.size();

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> rndx(0,nx);
	std::uniform_int_distribution<> rndy(0,ny);

	for(int i=0;i<300;++i) {
		int x=rndx(mt), y=rndy(mt);
		if(placeable(board, x, y)) 
			return std::make_tuple(x,y);
	}

	return std::make_tuple(rndx(mt),rndy(mt));
}
