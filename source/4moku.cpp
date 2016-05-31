#include "4moku.hpp"

void Board::reset(const int n_x, const int n_y, const int n_players) {
	xnum = n_x;
	ynum = n_y;
	data.clear();
	data.assign(xnum*ynum, 0);
	num_players = n_players;
}

int Board::operator()(int x, int y) const {
	return data[x + y*xnum];
}

int& Board::operator()(int x, int y) {
	return data[x + y*xnum];
}

const std::tuple<int, int> Board::size() const {
	return std::make_tuple(xnum, ynum);
}

std::size_t Board::size_x() const {
	return xnum;
}

std::size_t Board::size_y() const {
	return ynum;
}

int Board::players() const {
	return num_players;
}

int player_id(int player){
	return player+1;
}

bool placeable(const Board& board, int x, int y) {
	int nx,ny;
	std::tie(nx,ny) = board.size();

	// board範囲外はおけない
	if(x<0 || y<0 || nx<=x || ny<=y)
		return false;

	// すでに置かれているところには置けない
	if(board(x,y)!=0)
		return false;

	// 下はpieceまたは壁でなければならない
	// (2段目以上なら下にpieceがなければならない)
	if(y>0 && board(x,y-1)==0)
		return false;
	
	return true;
}

int finished(const Board& board) {
	int nx,ny;
	std::tie(nx,ny) = board.size();

	auto f = [&](int x,int y, int dx, int dy) {
		const int piece = board(x,y);
		if(piece==0)
			return 0;
		for(int i=1;i<4;++i) {
			if(x+dx*i>=nx || x+dx*i<0 || y+dy*i>=ny || y+dy*i<0)
				return 0;
			if(piece!=board(x+dx*i,y+dy*i))
				return 0;
		}
		return piece;
	};

	for(int i=0;i<nx;++i) {
		for(int j=0;j<ny;++j) {
			// 右方向
			const auto right = f(i,j,1,0);
			if(right>0) return right;

			// 下方向
			const auto down = f(i,j,0,-1);
			if(down>0) return down;

			// 右下方向
			const auto rightdown = f(i,j,1,-1);
			if(rightdown>0) return rightdown;

			// 右上方向
			const auto rightup= f(i,j,1,1);
			if(rightup>0) return rightup;
		}
	}
	return 0;
}

int update(Board& board,const int player,
			std::function<std::tuple<int,int>(const Board&,int)> ai_function) 
{
	int x,y;
	
	Board board_tmp(board);
	std::tie(x,y) = ai_function(board_tmp,player);
	if(placeable(board,x,y)) {
		board(x,y) = player_id(player);
	}
	else {
		// this player failed
		return FAILED;
	}

	if(finished(board)>0) {
		// win
		return WIN;
	}
	
	return 0;
}
