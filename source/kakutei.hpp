#include "4moku.hpp"

struct Kakutei{
private:
	std::mt19937 mt;
	std::uniform_real_distribution<double> rnd;
	
public:
	// �R���X�g���N�^�ł́A���������ɕK�v�Ȃ��̂����������Ă���
	Kakutei(int seed) : mt(seed + std::time(0)), rnd(0.0, 1.0) {}
	
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
	
	// AI�̓��e
	std::tuple<int, int> ai(const Board& board,int player) {
		// �E�����������Ƃ��m�肷��悤�Ȓu���ꏊ������Ȃ炻���ɒu���B
		// �E�������u�����ƂŁA���ɑ��肪�����Ƃ��m�肷��u���ꏊ������Ȃ炻���͒u���Ȃ��B
		// �E�����łȂ���΁A�u����ꏊ�ɒu���B
		
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
			
			// �����_���I���̏ꏊ�������I�΂�Ă��Ȃ��ꍇ�A�I��ł���
			// �i�G���[�ɂȂ�Ȃ��悤�ɂ��邽�߁j
			if(std::get<0>(random_choice) == -1){
				random_choice = this_choice;
			}
			
			// �����ɒu�����ƂŎ����̏������m��ł���Ȃ�u��
			Board b_tmp(b);
			b_tmp(i, j) = player_id(player);
			if(finished(b_tmp) == player_id(player)){
				final_result = this_choice;
				return false;
			}
			
			// �����ɒu�����ƂŁA���ɑ��肪�u���ď��Ă�ꏊ������Ȃ炻���ɒu���Ȃ�
			// ���肪�u���ꏊ�̌���(p, q)�Ƃ��A���ׂĂ̒u���ꏊ�ɂ��Ď����Ă���B
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
			
			// �����ɒu���ĕ������m�肵�Ȃ��ꍇ�A�����_���I���̌��ɉ�����B
			// 
			// �������A�u����ꏊ�����ӏ����邩�͍ŏ�����͂킩��Ȃ��̂ŁA
			// �ȉ��̕��@�őI������B
			// �E1�ڂɌ��������u����ꏊ�ɂ��ẮA�m��1�őI�ԁB
			// �E2�ڂɌ��������u����ꏊ�ɂ��ẮA�m��1/2�őI�ԁB
			// �E3�ڂɌ��������u����ꏊ�ɂ��ẮA�m��1/3�őI�ԁB
			//   :
			// �����߂̒u����ꏊ�ł��邩�́Acandidates�ɕێ����Ă���B
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
