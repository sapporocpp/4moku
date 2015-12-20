#include<iostream>
#include<algorithm>
#include<random>
#include<string>
#include<memory>
#include<vector>
#include<locale>
#include<functional>

struct Board;

const int WIN=1;
const int FAILED=2;
std::function<std::tuple<int,int>(const Board&,int)> ai[8];

struct Board {
	Board(const int xnum, const int ynum) : 
		xnum(xnum),ynum(ynum),data(xnum*ynum) 
	{ }

	int operator()(int x, int y) const {
		return data[x + y*xnum];
	}

	int& operator()(int x, int y) {
		return data[x + y*xnum];
	}

	const std::tuple<int,int> size() const {
		return std::make_tuple(xnum,ynum);
	}

	private:
	std::size_t xnum, ynum;
	std::vector<int> data;
};

void disp(const Board& board) {
	const std::vector<std::string> marker{" ","◯","□","△","●","■","▲","◇","◆"};
	int nx,ny;
	std::tie(nx,ny) = board.size();
	for(int j=ny-1;j>=0;--j) {
		// 何も置かれてない行は表示させない
		bool empty_line = true;
		for(int i=0;i<nx;++i)
			empty_line &= board(i,j)==0;
		if(empty_line)
			continue;

		std::string line;
		for(int i=0;i<nx;++i) {
			line += marker[board(i,j)];
		}
		std::cout << line << std::endl;
	}
	std::cout << std::endl;
}

int player_id(int player) {
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
			if(x+dx*i>=nx || y+dy*i>=ny)
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
	std::tie(x,y) = ai_function(board,player);
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

std::tuple<int,int> test_ai(const Board& board,int /*player*/) {
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

int main() {
	const auto xnum = 10, ynum=5;
	Board board = {xnum, ynum};
	for(int i=0;i<8;++i) ai[i]=test_ai;

	const int num_players = 4;
	while(true){
		int player,state;
		for(player=0;player<num_players;++player) {
			state = update(board, player, ai[player]);
			if(state!=0) {
				break;
			}
		}
		disp(board);
		std::cout << "-------------------------" << std::endl;
		if(state!=0) {
			if(state==WIN) 
				std::cout << "WIN player: " << player;
			if(state==FAILED) 
				std::cout << "FAILED player: " << player;
			break;
		}
	}

	return 0;
}


