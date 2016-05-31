#include "4moku.hpp"
#include<cstdlib>

// ここに使うAIを定義する
#include "test_ai.hpp"
#include "random_ai2.hpp"
#include "ai_winning.hpp"
#include "kakutei.hpp"
#include "ignis-neighbors.hpp"
#include "kakutei-more.hpp"

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

// メイン
int main() {
	using FuncType = std::tuple<int,int>(const Board&,int);
	using std::placeholders::_1;
	using std::placeholders::_2;

	// クラスで実装した場合はここでインスタンス化
	auto ai0 = TestAI();
	auto ai1 = TestAI();
	auto kakutei_ai1 = Kakutei(3290431);
	auto kakutei_ai2 = Kakutei(6490645);

	// 使うAIを登録
	std::vector<std::function<FuncType>> ai_list {
		/*
		[&](const Board& board, int player){ // ラムダ式で登録
			return ai0(board, player);
		},
		std::bind(&TestAI::operator(), ai1, _1, _2), // std::bindで登録
		ai_winning, // 関数ポインタで登録
		random_ai2,
		*/
			[&](const Board& board, int player){ return kakutei_ai1.ai(board, player); },
			[&](const Board& board, int player){ return kakutei_ai2.ai(board, player); }
		//std::bind(&TestAI::operator(), ai1, _1, _2)
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
