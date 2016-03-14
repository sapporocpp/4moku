#include "4moku.hpp"
#include<cstdlib>

// AIを定義したヘッダ
#include "test_ai.hpp"
#include "ai_winning.hpp"

int Board::operator()(int x, int y) const {
	return data[x + y*xnum];
}

int& Board::operator()(int x, int y) {
	return data[x + y*xnum];
}

const std::tuple<int,int> Board::size() const {
	return std::make_tuple(xnum,ynum);
}

int Board::players() const {
	return num_players;
}

void disp(const Board& board) {
	const std::vector<std::string> marker{"  ","A ","B ","C ","D ","E ","F ","G ","H "};

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

// メイン
int main() {
	using FuncType = std::tuple<int,int>(const Board&,int);
	using std::placeholders::_1;
	using std::placeholders::_2;

	// クラスで実装した場合はここでインスタンス化
	auto ai0 = TestAI();
	auto ai1 = TestAI();

	// 使うAIを登録
	std::vector<std::function<FuncType>> ai_list {
		[&](const Board& board, int player){ // ラムダ式で登録
			return ai0(board, player);
		},
		std::bind(&TestAI::operator(), ai1, _1, _2), // std::bindで登録
		ai_winning, // 関数ポインタで登録
	};

	const int num_players = ai_list.size();
	const auto xnum = 10, ynum=5;
	Board board = {xnum, ynum, num_players};
	
	// メインループをラムダ式で定義
	auto main_loop = [&]() {
		while(true){
			for(auto player=0;player<num_players;++player) {
				const auto state = update(board, player, ai_list[player]);
				disp(board);
				if(state!=0) {
					return std::make_pair(player, state);
				}
			}
		}
	};
	int last_player, last_state;
	std::tie(last_player, last_state) = main_loop(); // メインループを実行してるのはここ

	std::cout << "-------------------------" << std::endl;
	if(last_state==WIN)
		std::cout << "WIN player: " << last_player << std::endl;
	else if(last_state==FAILED)
		std::cout << "FAILED player: " << last_player << std::endl;
	else
		std::cout << "Invalid" << std::endl;
	return 0;
}

