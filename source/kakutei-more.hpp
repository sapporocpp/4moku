#include "4moku.hpp"
#include <deque>

// #define KAKUTEI_MORE_DEBUG

template<size_t SIMULATED_MOVES> struct KakuteiMore{
	/* SIMULATED_MOVES は、先読みする手数 */
	
private:
	std::mt19937 mt;
	std::uniform_real_distribution<double> rnd;
	
public:
	/* コンストラクタでは、乱数生成に必要なものを初期化しておく */
	KakuteiMore(int seed) : mt(seed + std::time(0)), rnd(0.0, 1.0) {}
	
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
	
	/* AIの先読みの結果 */
	struct AIresult{
		int winner;
		/*
		勝敗。
		0：未確定
		正の数：(SIMULATED_MOVES - winner + 1)手先で自分の勝ち
		負の数：(SIMULATED_MOVES - |winner| + 1)手先で他のプレイヤーの勝ち
		この数が多い手を優先的に選ぶ。
		*/
		
		std::tuple<int, int> position; /* 置く場所 */
		
		AIresult() : winner(0), position(std::make_tuple(-1, -1)) {}
		AIresult(int win, const std::tuple<int, int> & pos) : winner(win), position(pos) {}
		AIresult(int win, int pos_x, int pos_y) : winner(win), position(std::make_tuple(pos_x, pos_y)) {}
		
		explicit operator bool() const{
			return(std::get<0>(position) >= 0);
		}
	};
	
	/* ヘルパークラス：自分の指し手と、先読みしている手の関係 */
	struct BoardCandidate{
		Board board;
		AIresult choice;
		BoardCandidate(const Board & b, const AIresult & c) : board(b), choice(c) {}
	};
	
	/* AIの実質的な内容 */
	AIresult think(const Board& board, int this_player, size_t remained_depth){
		int nx, ny;
		std::tie(nx, ny) = board.size();
		
		std::deque<AIresult> position2score;
		
		/* 一手目（自分の手） */
		//try_for_placeable(board, [&](const Board & b, int i, int j){ ... });
		for(int i = 0; i < nx; ++i){ for(int j = 0; j < ny; ++j){
			if(!placeable(board, i, j)) continue;
			
			/* そこに置くことで勝ちが確定する？ */
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
			
			/* 二手目以降（各相手プレイヤーについて試す） */
			std::deque<BoardCandidate> current_candidates, next_candidates;
			current_candidates.emplace_back(b_tmp, AIresult(0, i, j));
			bool won_by_other = false;
			
			for(int moves = 1; moves < board.players(); ++moves){
				/* 誰が手を選ぶか */
				const int simulated_player = (this_player + moves) % board.players();
				
				/* 現在可能性のあるすべての手について調べる */
				for(auto cand = current_candidates.begin(); cand != current_candidates.end(); ++cand){
					try_for_placeable(cand->board, [&](const Board & b, int i1, int j1){
						/* そこに置くことで相手の勝ちが確定する？ */
						Board b_tmp(b);
						b_tmp(i1, j1) = player_id(simulated_player);
						if(finished(b_tmp) == player_id(simulated_player)){
#ifdef KAKUTEI_MORE_DEBUG
							if(remained_depth == SIMULATED_MOVES){
								std::cerr << "Player " << this_player << ": Placing at ("
								<< i << ", " << j << ") lets the player " << simulated_player << " win!" << std::endl;
							}
#endif // KAKUTEI_MORE_DEBUG
							
							/* 他のプレイヤーの勝ち */
							/* （その手は指せない、というフラグを立てる） */
							won_by_other = true;
							return false;
						}
						/* 探索継続 */
						next_candidates.emplace_back(b_tmp, cand->choice);
						return true;
					});
					if(won_by_other) break;
				}
				if(won_by_other) break;
				
				next_candidates.swap(current_candidates);
				next_candidates.clear();
			}
			
			/* この時点で決着が付かない場合、まだ先読みできる手数が残っているならば */
			/* 先読みする。 */
			if(!won_by_other){
				if(remained_depth > 1){
					/*
					まだ深く探索してもよいのであれば
					まだ残っている各可能性について検討する。
					これは、相手の手それぞれについてループするものなので、
					スコアは最悪のケースで評価しないとならない。
					*/

					int opponents_best_score = static_cast<int>(SIMULATED_MOVES);
					for(auto cand = current_candidates.begin(); cand != current_candidates.end(); ++cand){
						AIresult res = think(cand->board, this_player, remained_depth-1);
						if(res.winner < opponents_best_score){
							opponents_best_score = res.winner;
						}
					}
					
					position2score.emplace_back(opponents_best_score, i, j);
				}else{
					/* もうこれ以上の先読みができない場合（勝敗は不明） */
					position2score.emplace_back(0, i, j);
				}
			}else{
				/* 相手の勝ちが確定 */
				position2score.emplace_back(-remained_depth, i, j);
			}
		}} /* 「一手目」ループの終わり */
		
		/* 候補として残ったもののうち、最善のものを返す */
		if(position2score.empty()){
			/* 候補がもうない（＝盤面が埋まっている）ならば */
#ifdef KAKUTEI_MORE_DEBUG
			if(remained_depth == SIMULATED_MOVES){
				std::cerr << "No place remained!" << std::endl;
			}
#endif // KAKUTEI_MORE_DEBUG
			return AIresult(0, -1, -1);
		}
		
		int best_score = -(static_cast<int>(SIMULATED_MOVES)+1);
		double count_for_current_score = 0.0; /* 今のスコアの手がいくつあるか */
		AIresult chosen;
		std::deque<std::tuple<int, int>> uncertain_positions; /* 勝ち負けが決まっていない手。 */
		/* 勝ち負けが決まらない場合、別の基準で手を選ぶため、 */
		/* 別途保存しておく。 */
		
		for(auto it = position2score.begin(); it != position2score.end(); ++it){
			if(it->winner < best_score){
				/* この手が、すでに選ばれている手よりも悪い場合 */
				continue;
			}
			
			/* スコア更新、確率更新 */
			if(it->winner > best_score){
				best_score = it->winner;
				count_for_current_score = 1.0;
			}else{
				count_for_current_score += 1.0;
			}
			
			if(it->winner == 0){
				/* 勝ち負けが決まっていない手の場合：一度保存しておく */
				uncertain_positions.push_back(it->position);
			}else{
				/* 勝ち負けが決まっている手の場合：ランダム選択 */
				double r = rnd(mt);
				if(r <= 1.0 / count_for_current_score){
					chosen = *it;
				}
			}
		}
#ifdef KAKUTEI_MORE_DEBUG
		if(remained_depth == SIMULATED_MOVES && best_score > 0){
			std::cerr << "Player " << this_player << " can win by placing at ("
			<< std::get<0>(chosen.position) << ", " << std::get<1>(chosen.position) << ")! (*)" << std::endl;
		}
#endif // KAKUTEI_MORE_DEBUG
		
		/* 勝ち負けが決まっている手の場合、ランダム選択したものをそのまま返す */
		if(best_score != 0) return chosen;
		
		/*
		勝ち負けが決まっていない手の場合、
		1. 自分の石が3つ並ぶ（1つ間があってもよい）が、次に「相手が放置すると自分が勝てる」状況ではない
		2. 自分の石が2つ連続して並ぶ
		以上の順で優先的に選ぶ。
		*/
		return AIresult(0, uncertain_positions.front());
		/*
		for(auto it = uncertain_positions.begin(); it != uncertain_positions.end(); ++it){
			Board b_tmp(board);
			b_tmp(i, j) = player_id(this_player);
			
			// 1.の判定条件
			// 例えば左右方向にこの条件が満たされているか判定したい場合、
			// いま置いた石を起点に以下のことを調べればよい。
			// -A: 右に伸ばして、間の空いていない形で自分の石が何個連続しているか
			// -B: 右に伸ばして、間を1つまで許す形で自分の石が何個連続しているか
			// -C: 左に伸ばして、間の空いていない形で自分の石が何個連続しているか
			// -D: 左に伸ばして、間を1つまで許す形で自分の石が何個連続しているか
			// このとき、以下のように判定すればよい。
			// 
			// case(i): A + C >= 2
			// この場合、間のない状態で自分の石が3個以上並んでいる。
			// そのため、 blocking(A) と blocking(C) のどちらか一方でもが
			// 「空欄で、かつその真下に石がない」ものであればよい。
			// ただし blocking とは、その連続が途切れた場所（その場所はもう
			// 自分の石ではない）を意味する。
			// 
			// case(ii): A + D >= 2 (B + C >= 2 でも同様)
			// この場合、間のある状態で自分の石が3個以上並んでいる。
			// そのため、 blocking(C) （これは必然的に空欄になる）の
			// 真下に石がないものであればよい。
			
		}
		*/
	}
	
	/* AIの内容 */
	std::tuple<int, int> ai(const Board& board, int player) {
		AIresult th = think(board, player, SIMULATED_MOVES);
		return th.position;
	}
};
