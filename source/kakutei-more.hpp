#include "4moku.hpp"
#include <deque>

// #define KAKUTEI_MORE_DEBUG

template<size_t SIMULATED_MOVES> struct KakuteiMore{
	// SIMULATED_MOVES �́A��ǂ݂���萔
	
private:
	std::mt19937 mt;
	std::uniform_real_distribution<double> rnd;
	
public:
	// �R���X�g���N�^�ł́A���������ɕK�v�Ȃ��̂����������Ă���
	KakuteiMore(int seed) : mt(seed + std::time(0)), rnd(0.0, 1.0) {}
	
	// �w���p�[�֐��F
	// �Ֆʂ̂����u����ꏊ���ׂĂɂ��ČJ��Ԃ�
	// false��Ԃ����ꍇ���̎��_�őł��؂�
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
	
	// AI�̐�ǂ݂̌���
	struct AIresult{
		int winner;
		// ���s�B
		// 0�F���m��
		// ���̐��F(SIMULATED_MOVES - winner + 1)���Ŏ����̏���
		// ���̐��F(SIMULATED_MOVES - |winner| + 1)���ő��̃v���C���[�̏���
		// ���̐����������D��I�ɑI�ԁB
		
		std::tuple<int, int> position; // �u���ꏊ
		
		AIresult() : winner(0), position(std::make_tuple(-1, -1)) {}
		AIresult(int win, const std::tuple<int, int> & pos) : winner(win), position(pos) {}
		AIresult(int win, int pos_x, int pos_y) : winner(win), position(std::make_tuple(pos_x, pos_y)) {}
		
		explicit operator bool() const{
			return(std::get<0>(position) >= 0);
		}
	};
	
	// �w���p�[�N���X�F�����̎w����ƁA��ǂ݂��Ă����̊֌W
	struct BoardCandidate{
		Board board;
		AIresult choice;
		BoardCandidate(const Board & b, const AIresult & c) : board(b), choice(c) {}
	};
	
	// AI�̎����I�ȓ��e
	AIresult think(const Board& board, int this_player, size_t remained_depth){
		int nx, ny;
		std::tie(nx, ny) = board.size();
		
		std::deque<AIresult> position2score;
		//std::deque<std::tuple<int, int>> uncertain_positions;
		//std::deque<AIresult> future_winning_positions;
		
		// ���ځi�����̎�j
		//try_for_placeable(board, [&](const Board & b, int i, int j){ ... });
		for(int i = 0; i < nx; ++i){ for(int j = 0; j < ny; ++j){
			if(!placeable(board, i, j)) continue;
			
			// �����ɒu�����Ƃŏ������m�肷��H
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
			
			// ���ڈȍ~�i�e����v���C���[�ɂ��Ď����j
			std::deque<BoardCandidate> current_candidates, next_candidates;
			current_candidates.emplace_back(b_tmp, AIresult(0, i, j));
			bool won_by_other = false;
			
			for(int moves = 1; moves < board.players(); ++moves){
				// �N�����I�Ԃ�
				const int simulated_player = (this_player + moves) % board.players();
				
				// ���݉\���̂��邷�ׂĂ̎�ɂ��Ē��ׂ�
				for(auto cand = current_candidates.begin(); cand != current_candidates.end(); ++cand){
					try_for_placeable(cand->board, [&](const Board & b, int i1, int j1){
						// �����ɒu�����Ƃő���̏������m�肷��H
						Board b_tmp(b);
						b_tmp(i1, j1) = player_id(simulated_player);
						if(finished(b_tmp) == player_id(simulated_player)){
#ifdef KAKUTEI_MORE_DEBUG
							if(remained_depth == SIMULATED_MOVES){
								std::cerr << "Player " << this_player << ": Placing at ("
								<< i << ", " << j << ") lets the player " << simulated_player << " win!" << std::endl;
							}
#endif // KAKUTEI_MORE_DEBUG
							
							// ���̃v���C���[�̏���
							// �i���̎�͎w���Ȃ��A�Ƃ����t���O�𗧂Ă�j
							won_by_other = true;
							return false;
						}
						// �T���p��
						next_candidates.emplace_back(b_tmp, cand->choice);
						return true;
					});
					if(won_by_other) break;
				}
				if(won_by_other) break;
				
				next_candidates.swap(current_candidates);
				next_candidates.clear();
			}
			
			// ���̎��_�Ō������t���Ȃ��ꍇ�A�܂���ǂ݂ł���萔���c���Ă���Ȃ��
			// ��ǂ݂���B
			if(!won_by_other){
				if(remained_depth > 1){
					// �܂��[���T�����Ă��悢�̂ł����
					// �܂��c���Ă���e�\���ɂ��Č�������B
					// ����́A����̎肻�ꂼ��ɂ��ă��[�v������̂Ȃ̂ŁA
					// �X�R�A�͍ň��̃P�[�X�ŕ]�����Ȃ��ƂȂ�Ȃ��B
					int opponents_best_score = SIMULATED_MOVES;
					for(auto cand = current_candidates.begin(); cand != current_candidates.end(); ++cand){
						AIresult res = think(cand->board, this_player, remained_depth-1);
						if(res.winner < opponents_best_score){
							opponents_best_score = res.winner;
						}
					}
					
					position2score.emplace_back(opponents_best_score, i, j);
				}else{
					// ��������ȏ�̐�ǂ݂��ł��Ȃ��ꍇ�i���s�͕s���j
					position2score.emplace_back(0, i, j);
				}
			}else{
				// ����̏������m��
				position2score.emplace_back(-remained_depth, i, j);
			}
		}} // �u���ځv���[�v�̏I���
		
		// ���Ƃ��Ďc�������̂̂����A�őP�̂��̂�Ԃ�
		if(position2score.empty()){
			// ��₪�����Ȃ��i���Ֆʂ����܂��Ă���j�Ȃ��
#ifdef KAKUTEI_MORE_DEBUG
			if(remained_depth == SIMULATED_MOVES){
				std::cerr << "No place remained!" << std::endl;
			}
#endif // KAKUTEI_MORE_DEBUG
			return AIresult(0, -1, -1);
		}
		
		int best_score = -(static_cast<int>(SIMULATED_MOVES)+1);
		double count_for_current_score = 0.0; // ���̃X�R�A�̎肪�������邩
		AIresult chosen;
		
		for(auto it = position2score.begin(); it != position2score.end(); ++it){
			if(it->winner < best_score){
				// ���̎肪�A���łɑI�΂�Ă������������ꍇ
				continue;
			}
			
			if(it->winner > best_score){
				best_score = it->winner;
				count_for_current_score = 1.0;
			}else{
				count_for_current_score += 1.0;
			}
			
			double r = rnd(mt);
			if(r <= 1.0 / count_for_current_score){
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
	
	// AI�̓��e
	std::tuple<int, int> ai(const Board& board, int player) {
		AIresult th = think(board, player, SIMULATED_MOVES);
		return th.position;
	}
};
