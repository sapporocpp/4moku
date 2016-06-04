#include "4moku.hpp"
#include <deque>
#include <ctime>
#include <cstdlib>
#include <iostream>

// #define KAKUTEI_MORE_DEBUG

template<size_t SIMULATED_MOVES> struct KakuteiMore{
	// SIMULATED_MOVES は、先読みする手数
	
private:
	std::mt19937 mt;
	std::uniform_real_distribution<double> rnd;
	
public:
	// コンストラクタでは、乱数生成に必要なものを初期化しておく
	KakuteiMore() : mt(std::time(nullptr)), rnd(0.0, 1.0) {
		std::srand(std::time(nullptr));
	}
	KakuteiMore(int seed) : mt(seed + std::time(nullptr)), rnd(0.0, 1.0) {
		std::srand(std::time(nullptr));
	}

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
		
		std::deque<AIresult> position2score;
		//std::deque<std::tuple<int, int>> uncertain_positions;
		//std::deque<AIresult> future_winning_positions;
		
		// 一手目（自分の手）
		//try_for_placeable(board, [&](const Board & b, int i, int j){ ... });
		for(int i = 0; i < nx; ++i){ for(int j = 0; j < ny; ++j){
			if(!placeable(board, i, j)) continue;
			
			// そこに置くことで勝ちが確定する？
			Board b_tmp(board);
			b_tmp(i, j) = player_id(this_player);
			if(finished(b_tmp) == player_id(this_player)){
#ifdef KAKUTEI_MORE_DEBUG
			if(remained_depth == SIMULATED_MOVES){
				std::cerr << "Player " << this_player << " can win by placing at ("
				<< i << ", " << j << ")!" << std::endl;
			}
#endif // KAKUTEI_MORE_DEBUG
				return AIresult(remained_depth, i, j);
			}
			
			// 二手目以降（各相手プレイヤーについて試す）
			std::deque<BoardCandidate> current_candidates, next_candidates;
			current_candidates.emplace_back(b_tmp, AIresult(0, i, j));
			bool won_by_other = false;
			
			for(int moves = 1; moves < board.players(); ++moves){
				// 誰が手を選ぶか
				const int simulated_player = (this_player + moves) % board.players();
				typename std::deque<BoardCandidate>::iterator cand;
				
				// 現在可能性のあるすべての手について調べる
				for (cand = current_candidates.begin(); cand != current_candidates.end(); ++cand) {
					try_for_placeable(cand->board, [&](const Board & b, int i1, int j1){
						// そこに置くことで相手の勝ちが確定する？
						Board b_tmp(b);
						b_tmp(i1, j1) = player_id(simulated_player);
						if(finished(b_tmp) == player_id(simulated_player)){
#ifdef KAKUTEI_MORE_DEBUG
							if(remained_depth == SIMULATED_MOVES){
								std::cerr << "Player " << this_player << ": Placing at ("
								<< i << ", " << j << ") lets the player " << simulated_player << " win!" << std::endl;
							}
#endif // KAKUTEI_MORE_DEBUG
							
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
			
			// この時点で決着が付かない場合、まだ先読みできる手数が残っているならば
			// 先読みする。
			if(!won_by_other){
				if(remained_depth > 1){
					// まだ深く探索してもよいのであれば
					// まだ残っている各可能性について検討する。
					// これは、相手の手それぞれについてループするものなので、
					// スコアは最悪のケースで評価しないとならない。
					int opponents_best_score = SIMULATED_MOVES;
					typename std::deque<BoardCandidate>::iterator cand;
					for (cand = current_candidates.begin(); cand != current_candidates.end(); ++cand) {
						AIresult res = think(cand->board, this_player, remained_depth-1);
						if(res.winner < opponents_best_score){
							opponents_best_score = res.winner;
						}
					}
					
					position2score.emplace_back(opponents_best_score, i, j);
				}else{
					// もうこれ以上の先読みができない場合（勝敗は不明）
					position2score.emplace_back(0, i, j);
				}
			}else{
				// 相手の勝ちが確定
				position2score.emplace_back(-static_cast<int>(remained_depth), i, j);
			}
		}} // 「一手目」ループの終わり
		
		// 候補として残ったもののうち、最善のものを返す
		if(position2score.empty()){
			// 候補がもうない（＝盤面が埋まっている）ならば
#ifdef KAKUTEI_MORE_DEBUG
			if(remained_depth == SIMULATED_MOVES){
				std::cerr << "No place remained!" << std::endl;
			}
#endif // KAKUTEI_MORE_DEBUG
			return AIresult(0, -1, -1);
		}
		
		int best_score = -(static_cast<int>(SIMULATED_MOVES)+1);
		double count_for_current_score = 0.0; // 今のスコアの手がいくつあるか
		AIresult chosen;
		typename std::deque<AIresult>::iterator it;
		
		for(it = position2score.begin(); it != position2score.end(); ++it){
			if(it->winner < best_score){
				// この手が、すでに選ばれている手よりも悪い場合
				continue;
			}
			
			if(it->winner > best_score){
				best_score = it->winner;
				count_for_current_score = 1.0;
			}else{
				count_for_current_score += 1.0;
			}
			
			/*
			double r = rnd(mt);
			if(r <= 1.0 / count_for_current_score){
				chosen = *it;
			}
			*/
			int r = std::rand();
			if (r <= RAND_MAX / count_for_current_score) {
				chosen = *it;
			}
		}
#ifdef KAKUTEI_MORE_DEBUG
		if(remained_depth == SIMULATED_MOVES && best_score > 0){
			std::cerr << "Player " << this_player << " can win by placing at ("
			<< std::get<0>(chosen.position) << ", " << std::get<1>(chosen.position) << ")! (*)" << std::endl;
		}
#endif // KAKUTEI_MORE_DEBUG
		return chosen;
	}
	
	// AIの内容
	std::tuple<int, int> ai(const Board& board, int player) {
		AIresult th = think(board, player, SIMULATED_MOVES);
		return th.position;
	}
};
