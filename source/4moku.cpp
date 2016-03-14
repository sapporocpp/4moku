#include "4moku.hpp"
#include<cstdlib>

// 定義されているAIの数
constexpr std::size_t NUM_AI = 10;

// 一度に対戦させられる数
constexpr std::size_t NUM_PLAY = 8;

// ここに使うAIを定義する
#define AI_FUNCTION AIFunction1
#include "kakutei.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction2
#include "ignis.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction3
#include "test_ai.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction4
#include "test_ai.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction5
#include "test_ai.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction6
#include "test_ai.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction7
#include "test_ai.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction8
#include "test_ai.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction9
#include "test_ai.hpp"
#undef AI_FUNCTION
#define AI_FUNCTION AIFunction10
#include "test_ai.hpp"
#undef AI_FUNCTION

int Board::operator()(int x, int y) const {
	return data[x + y*xnum];
}

int& Board::operator()(int x, int y) {
	return data[x + y*xnum];
}

const std::tuple<int,int> Board::size() const {
	return std::make_tuple(xnum,ynum);
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

// コマンドライン引数の説明を表示して終了
void usage_and_exit(){
	std::cerr << "Usage: 4moku [SIZE_X] [SIZE_Y] [AI_ID1] [AI_ID2] ..." << std::endl;
	std::cerr << "    Specify [SIZE_X] and [SIZE_Y] with integers 2 or larger." << std::endl;
	std::cerr << "    Specify every [AI_ID#] with an integer between 1 to " << NUM_AI << "." << std::endl;
	std::cerr << "    Example: 4moku 5 2 1 3 1 2" << std::endl;
	std::exit(-1);
}

// メイン
int main(int argc, char ** argv) {
	std::function<std::tuple<int,int>(const Board&,int)> ai_list[NUM_AI] = {
		AIFunction1,
		AIFunction2,
		AIFunction3,
		AIFunction4,
		AIFunction5,
		AIFunction6,
		AIFunction7,
		AIFunction8,
		AIFunction9,
		AIFunction10};
	
	// コマンドライン引数の解析(1) 引数の数
	if(argc <= 4) usage_and_exit();
	
	// コマンドライン引数の解析(2) 盤面の大きさ
	const auto xnum = std::atoi(argv[1]);
	const auto ynum = std::atoi(argv[2]);
	if(xnum <= 1 || ynum <= 1) usage_and_exit();
	
	// コマンドライン引数の解析(3) 各プレイヤーがどのAIを使うか
	const int num_players = argc - 3;
	std::vector< std::function<std::tuple<int,int>(const Board&,int)> > ai(num_players);
	
	for(int player = 0; player < num_players; ++player){
		std::size_t ai_id = static_cast<std::size_t>(std::atoi(argv[player+3]));
		if(ai_id <= 0 || ai_id > NUM_AI) usage_and_exit();
		ai[player] = ai_list[ai_id-1];
		std::cerr << "Player " << player << " plays with AI " << ai_id << "." << std::endl;
	}
	
	// 対戦を実行する
	Board board = {xnum, ynum};
	
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

