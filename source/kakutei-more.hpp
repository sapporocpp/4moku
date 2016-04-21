#include "4moku.hpp"
#include <deque>

template<size_t SIMULATED_MOVES> struct KakuteiMore{
	// SIMULATED_MOVES は、先読みする手数
	
private:
	std::mt19937 mt;
	std::uniform_real_distribution<double> rnd;
	std::tuple<int,int> final_result;
	
public:
	// コンストラクタでは、乱数生成に必要なものを初期化しておく
	KakuteiMore(int seed) : mt(seed + std::time(0)), rnd(0.0, 1.0) {}
	
	// ヘルパー関数：
	// 盤面のうち置ける場所すべてについて繰り返す
	// falseを返した場合その時点で打ち切る
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
	
	// AIの先読みの結果
	struct AIresult{
		int winner;
		// 勝敗。
		// 0：未確定
		// 正の数：(SIMULATED_MOVES - winner + 1)手先で自分の勝ち
		// 負の数：(SIMULATED_MOVES - |winner| + 1)手先で他のプレイヤーの勝ち
		// この数が多い手を優先的に選ぶ。
		
		std::tuple<int, int> position; // 置く場所
		
		AIresult() : winner(0), position(std::make_tuple(-1, -1)) {}
		AIresult(int win, const std::tuple<int, int> & pos) : winner(win), position(pos) {}
		AIresult(int win, int pos_x, int pos_y) : winner(win), position(std::make_tuple(pos_x, pos_y)) {}
		
		explicit operator bool() const{
			return(std::get<0>(position) >= 0);
		}
	};
	
	// ヘルパークラス：自分の指し手と、先読みしている手の関係
	struct BoardCandidate{
		Board board;
		AIresult choice;
		BoardCandidate(const Board & b, const AIresult & c) : board(b), choice(c) {}
	};
	
	// AIの実質的な内容
	AIresult think(const Board& board, int this_player, size_t remained_depth){
		int nx, ny;
		std::tie(nx, ny) = board.size();
		
		AIresult final_result;
		std::deque<std::tuple<int, int>> uncertain_positions;
		std::deque<AIresult> future_winning_positions;
		
		// 一手目（自分の手）
		//try_for_placeable(board, [&](const Board & b, int i, int j){
		for(int i = 0; i < nx; ++i){ for(int j = 0; j < ny; ++j){
			if(!placeable(board, i, j)) continue;
			
			// そこに置くことで勝ちが確定する？
			Board b_tmp(board);
			b_tmp(i, j) = player_id(this_player);
			if(finished(b_tmp) == player_id(this_player)){
if(remained_depth == SIMULATED_MOVES){
std::cerr << "Player " << this_player << " can win by placing at ("
	<< i << ", " << j << ")!" << std::endl;
}
				return AIresult(remained_depth, i, j);
			}
			
			// 二手目以降（各相手プレイヤーについて試す）
			std::deque<BoardCandidate> current_candidates, next_candidates;
			current_candidates.emplace_back(b_tmp, AIresult(0, i, j));
			bool won_by_other = false;
			
			for(int moves = 1; moves < board.players(); ++moves){
				// 誰が手を選ぶか
				const int simulated_player = (this_player + moves) % board.players();
				
				// 現在可能性のあるすべての手について調べる
				for(auto cand = current_candidates.begin(); cand != current_candidates.end(); ++cand){
					try_for_placeable(cand->board, [&](const Board & b, int i1, int j1){
						// そこに置くことで相手の勝ちが確定する？
						Board b_tmp(b);
						b_tmp(i1, j1) = player_id(simulated_player);
						if(finished(b_tmp) == player_id(simulated_player)){
if(remained_depth == SIMULATED_MOVES){
std::cerr << "Player " << this_player << ": Placing at ("
	<< i << ", " << j << ") lets the player " << simulated_player << " win!" << std::endl;
}
							// 他のプレイヤーの勝ち
							// （その手は指せない、というフラグを立てる）
							won_by_other = true;
							return false;
						}
						// 探索継続
						next_candidates.emplace_back(b_tmp, cand->choice);
						return true;
					});
					if(won_by_other) break;
				}
				if(won_by_other) break;
				
				next_candidates.swap(current_candidates);
				next_candidates.clear();
			}
			
			// 可能性についての確認。
			// 自分の負けが確定といえるのは
			// ・この時点の先読みで負けが確定した場合
			// ・次の段階の先読みで、すべての手で負けることが確定した場合
			// のどちらかに該当する場合である。
			// よって、この時点で負けておらず、かつ次の先読みをして負けない手があれば
			// uncertain_positionsに加える。
			// なお、次の先読みで勝てると決まった場合はそのままfinal_resultに加える。
			// 
			// また、もし先読みができない場合は、「この時点で先読み候補が残っていれば」
			// uncertain_positionsに加えればよい。
			if(!won_by_other){
				if(remained_depth > 1){
					// まだ深く探索してもよいのであれば
					// まだ残っている各可能性について検討する
					size_t num_winning = 0, num_uncertain = 0, num_losing = 0;
					for(auto cand = current_candidates.begin(); cand != current_candidates.end(); ++cand){
						AIresult res = think(cand->board, this_player, remained_depth-1);
						if(res.winner < 0){
							++num_losing;
							break;
						}else if(res.winner > 0){
							++num_winning;
						}else{
							++num_uncertain;
						}
					}
					
					if(num_winning == current_candidates.size()){
						// 相手がどの手を指しても勝てる場合
if(remained_depth == SIMULATED_MOVES){
std::cerr << "Player " << this_player << " can win by placing at ("
<< i << ", " << j << ")! (*)" << std::endl;
}
						future_winning_positions.push_back(AIresult(remained_depth, i, j));
					}else if(num_losing > 0){
						// 自分の負けを確定させる手を相手が持っている場合（何もしない）
if(remained_depth == SIMULATED_MOVES){
std::cerr << "Player " << this_player << ": Placing at ("
<< i << ", " << j << ") lets another player win! (*)" << std::endl;
}
					}else{
						// 未確定の場合（未確定リストに入れる）
						uncertain_positions.push_back(std::make_tuple(i, j));
					}
				}else{
					// もうこれ以上の先読みができない場合（未確定リストに入れる）
					if(!(current_candidates.empty())){
						uncertain_positions.push_back(std::make_tuple(i, j));
					}
				}
			}
		}} // 「一手目」ループの終わり
		
		// いまの手ではないが、この先に勝てる手がある場合
		if(!(future_winning_positions.empty())){
			// TODO: もっとも早く勝てる手を選ぶ
			return future_winning_positions.front();
		}
		
		// 勝敗の確定しない手があるなら、それをランダムに一つ返す
		if(!(uncertain_positions.empty())){
			if(remained_depth == SIMULATED_MOVES){
				double num_candidates = 0.0;
				
				for(size_t i = 0; i < uncertain_positions.size(); ++i){
					num_candidates += 1.0;
					double r = rnd(mt);
					if(r <= 1.0 / num_candidates){
						final_result = AIresult(0, uncertain_positions[i]);
					}
				}
				return final_result;
			}else{
				return AIresult(0, uncertain_positions.front());
			}
		}
		
		// それもないなら、負ける手を返す
		{
			double num_candidates = 0.0;
			
			try_for_placeable(board, [&](const Board & /* b */, int i, int j){
				num_candidates += 1.0;
				double r = rnd(mt);
				if(r <= 1.0 / num_candidates){
					final_result = AIresult(-remained_depth, i, j);
				}
				return true;
			});
			return final_result;
		}
	}
	
	// AIの内容
	std::tuple<int, int> ai(const Board& board, int player) {
		AIresult th = think(board, player, SIMULATED_MOVES);
		return th.position;
	}
};
