#include "4moku.hpp"
#include <deque>

template<size_t SIMULATED_MOVES> struct KakuteiMore{
	// SIMULATED_MOVES �́A��ǂ݂���萔
	
private:
	std::mt19937 mt;
	std::uniform_real_distribution<double> rnd;
	std::tuple<int,int> final_result;
	
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
		// 0�F���m��A���̐��F���̎萔��Ŏ����̏����A���̐��F���̎萔��ő��̃v���C���[�̏���
		
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
	AIresult think(const Board& board, int this_player, size_t depth){
		int nx, ny;
		std::tie(nx, ny) = board.size();
		
		AIresult final_result;
		std::deque<std::tuple<int, int>> uncertain_positions;
		std::deque<AIresult> future_winning_positions;
		
		// ���ځi�����̎�j
		//try_for_placeable(board, [&](const Board & b, int i, int j){
		for(int i = 0; i < nx; ++i){ for(int j = 0; j < ny; ++j){
			if(!placeable(board, i, j)) continue;
			
			// �����ɒu�����Ƃŏ������m�肷��H
			Board b_tmp(board);
			b_tmp(i, j) = player_id(this_player);
			if(finished(b_tmp) == player_id(this_player)){
if(depth == 0){
std::cerr << "Player " << this_player << " can win by placing at ("
	<< i << ", " << j << ")!" << std::endl;
}
				return AIresult(depth, i, j);
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
if(depth == 0){
std::cerr << "Player " << this_player << ": Placing at ("
	<< i << ", " << j << ") lets the player " << simulated_player << " win!" << std::endl;
}
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
			
			// �\���ɂ��Ă̊m�F�B
			// �����̕������m��Ƃ�����̂�
			// �E���̎��_�̐�ǂ݂ŕ������m�肵���ꍇ
			// �E���̒i�K�̐�ǂ݂ŁA���ׂĂ̎�ŕ����邱�Ƃ��m�肵���ꍇ
			// �̂ǂ��炩�ɊY������ꍇ�ł���B
			// ����āA���̎��_�ŕ����Ă��炸�A�����̐�ǂ݂����ĕ����Ȃ��肪�����
			// uncertain_positions�ɉ�����B
			// �Ȃ��A���̐�ǂ݂ŏ��Ă�ƌ��܂����ꍇ�͂��̂܂�final_result�ɉ�����B
			// 
			// �܂��A������ǂ݂��ł��Ȃ��ꍇ�́A�u���̎��_�Ő�ǂ݌�₪�c���Ă���΁v
			// uncertain_positions�ɉ�����΂悢�B
			if(!won_by_other){
				if(depth < SIMULATED_MOVES){
					// �܂��[���T�����Ă��悢�̂ł����
					// �܂��c���Ă���e�\���ɂ��Č�������
					size_t num_winning = 0, num_uncertain = 0, num_losing = 0;
					for(auto cand = current_candidates.begin(); cand != current_candidates.end(); ++cand){
						AIresult res = think(cand->board, this_player, depth+1);
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
						// ���肪�ǂ̎���w���Ă����Ă�ꍇ
if(depth == 0){
std::cerr << "Player " << this_player << " can win by placing at ("
<< i << ", " << j << ")! (*)" << std::endl;
}
						future_winning_positions.push_back(AIresult(depth, i, j));
					}else if(num_losing > 0){
						// �����̕������m�肳�����𑊎肪�����Ă���ꍇ�i�������Ȃ��j
if(depth == 0){
std::cerr << "Player " << this_player << ": Placing at ("
<< i << ", " << j << ") lets another player win! (*)" << std::endl;
}
					}else{
						// ���m��̏ꍇ�i���m�胊�X�g�ɓ����j
						uncertain_positions.push_back(std::make_tuple(i, j));
					}
				}else{
					// ��������ȏ�̐�ǂ݂��ł��Ȃ��ꍇ�i���m�胊�X�g�ɓ����j
					if(!(current_candidates.empty())){
						uncertain_positions.push_back(std::make_tuple(i, j));
					}
				}
			}
		}} // �u���ځv���[�v�̏I���
		
		// ���܂̎�ł͂Ȃ����A���̐�ɏ��Ă�肪����ꍇ
		if(!(future_winning_positions.empty())){
			// TODO: �����Ƃ��������Ă���I��
			return future_winning_positions.front();
		}
		
		// ���s�̊m�肵�Ȃ��肪����Ȃ�A����������_���Ɉ�Ԃ�
		if(!(uncertain_positions.empty())){
			if(depth == 0){
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
		
		// ������Ȃ��Ȃ�A��������Ԃ�
		{
			double num_candidates = 0.0;
			
			try_for_placeable(board, [&](const Board & /* b */, int i, int j){
				num_candidates += 1.0;
				double r = rnd(mt);
				if(r <= 1.0 / num_candidates){
					final_result = AIresult(-1, i, j);
				}
				return true;
			});
			return final_result;
		}
	}
	
	// AI�̓��e
	std::tuple<int, int> ai(const Board& board, int player) {
		AIresult th = think(board, player, 0);
		return th.position;
	}
};
