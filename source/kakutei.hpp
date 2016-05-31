#include "4moku.hpp"

struct Kakutei{
private:
	std::mt19937 mt;
	std::uniform_real_distribution<double> rnd;
	
public:
	/* コンストラクタでは、乱数生成に必要なものを初期化しておく */
	Kakutei(int seed) : mt(seed + std::time(0)), rnd(0.0, 1.0) {}
	
	/*
	ヘルパー関数：
	盤面のうち置ける場所すべてについて繰り返す
	falseを返した場合その時点で打ち切る
	*/
	template <class FUNCTYPE>
	void try_for_placeable(const Board & board, const FUNCTYPE & func){
		bool break_loop = false;
		
		int nx, ny;
		std::tie(nx, ny) = board.size();
		for(int i = 0; i < nx; ++i){
			for(int j = 0; j < ny; ++j){
				if(!placeable(board, i, j)) continue;
				
				if(!func(board, i, j)){
					break_loop = true;
					break;
				}
			}
			if(break_loop) break;
		}
	}
	
	/* AIの内容 */
	std::tuple<int, int> ai(const Board& board,int player) {
		/*
		・自分が勝つことが確定するような置き場所があるならそこに置く。
		・自分が置くことで、次に相手が勝つことが確定する置き場所があるならそこは置かない。
		・そうでなければ、置ける場所に置く。
		*/
		
		int nx,ny;
		std::tie(nx,ny) = board.size();
		std::tuple<int,int> random_choice, final_result;
		random_choice = std::make_tuple(-1, -1);
		final_result = std::make_tuple(-1, -1);
		
		int candidates = 0;
		
		auto& mt_i = mt;
		auto& rnd_i = rnd;
		
		try_for_placeable(board, [&](const Board & b, int i, int j){
			std::tuple<int,int> this_choice = std::make_tuple(i, j);
			
			/* ランダム選択の場所が何も選ばれていない場合、選んでおく */
			/* （エラーにならないようにするため） */
			if(std::get<0>(random_choice) == -1){
				random_choice = this_choice;
			}
			
			/* そこに置くことで自分の勝ちが確定できるなら置く */
			Board b_tmp(b);
			b_tmp(i, j) = player_id(player);
			if(finished(b_tmp) == player_id(player)){
				final_result = this_choice;
				return false;
			}
			
			/* そこに置くことで、次に相手が置いて勝てる場所があるならそこに置かない */
			/* 相手が置く場所の候補を(p, q)とし、すべての置き場所について試している。 */
			bool lost = false;
			try_for_placeable(b_tmp, [&](const Board & b_t, int p, int q){
				Board b_t2(b_t);
				b_t2(p, q) = player_id((player+1) % b_t.players());
				if(finished(b_t2)){
					lost = true;
					std::cerr << "Placing at (" << i << ", " << j << ") lets the opponent win!" << std::endl;
					return false;
				}
				return true;
			});
			
			/*
			そこに置いて負けが確定しない場合、ランダム選択の候補に加える。
			
			ただし、置ける場所が何箇所あるかは最初からはわからないので、
			以下の方法で選択する。
			・1つ目に見つかった置ける場所については、確率1で選ぶ。
			・2つ目に見つかった置ける場所については、確率1/2で選ぶ。
			・3つ目に見つかった置ける場所については、確率1/3で選ぶ。
			  :
			いくつめの置ける場所であるかは、candidatesに保持している。
			*/
			if(!lost){
				++candidates;
				
				double r = rnd_i(mt_i);
				//std::cout << "random: " << r << std::endl;
				if(r <= 1.0 / static_cast<double>(candidates)){
					random_choice = this_choice;
				}
			}
			return true;
		}); // try_for_placeable
		
		if(std::get<0>(final_result) != -1) return final_result;
		
		//std::cerr << "Random choice! "
		//	<< std::get<0>(random_choice) << ", "
		//	<< std::get<1>(random_choice) << std::endl;
		return random_choice;
	}
};
