#include "4moku.hpp"

/*
 ランダムに数手置いてみて、一番勝てそうなものを採用する。
 */


// ランダムに置くAI
std::tuple<int,int> random_ai(const Board& board,int /*player*/) {
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


// おける場所を列挙する
std::vector<std::tuple<int,int>> get_candidates(const Board& board) {
	int nx,ny;
	std::tie(nx,ny) = board.size();

	std::vector<std::tuple<int,int>> list;
	for(int x=0;x<nx;++x) {
		for(int y=0;y<ny;++y) {
			if(placeable(board, x, y)) {
				list.emplace_back(std::make_tuple(x,y));
				break;
			}
		}
	}
	return list;
}

// ランダムで最後まで試行し、勝ったかを調べる
template<typename Position>
bool get_try_result(Board board, int self, Position next) {
	// 自分の次の石を置く
	const int x = std::get<0>(next);
	const int y = std::get<1>(next);
	board(x,y) = player_id(self);

	// 勝ち決定したら即返す
	if(finished(board)>0) {
		return true;
	}

	// 次のプレイヤーからランダムに置かせる
	int player,state=0;
	const int num = board.players();

	// 終わるまでループ
	for(int i=0;;++i) {
		// 最初だけは、自分の次のプレイヤーから開始
		const int p = i==0 ? self+1 : 0;

		// 各プレイヤーのAI呼び出し
		// 他AIはわからないのでランダムAIを利用
		for(player=p;player<num;++player) {
			state = update(board, player, random_ai);
			if(state!=0) {
				break;
			}
		}

		// 勝敗を調べて勝っていたらtrueを返す
		if(state!=0) {
			if(state==WIN && player==self)
				return true;
			else
				return false;
		}
	}
}

std::tuple<int,int> random_ai2(const Board& board,int player) {
	// 次における場所の候補
	const auto candidates = get_candidates(board);
	
	// 候補１つに対する試行回数
	const int try_number = 100;

	// 勝率リスト 
	std::vector<double> p;
	
	for(auto pos: candidates) {
		int win_count=0;
		for(int i=0;i<try_number;++i) {
			if(get_try_result(board, player, pos)) {
				++win_count;
			}
		}
		p.push_back(win_count);
	}

	auto itr = std::max_element(p.begin(), p.end());
	auto idx = std::distance(p.begin(), itr);
	return candidates[idx];
}




