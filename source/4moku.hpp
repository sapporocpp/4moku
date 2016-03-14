#ifndef HPP_4MOKU
#define HPP_4MOKU

#include<iostream>
#include<algorithm>
#include<random>
#include<string>
#include<memory>
#include<vector>
#include<locale>
#include<functional>

struct Board;

const int WIN=1;
const int FAILED=2;

struct Board {
	// ��̔Ֆʂ𐶐�
	Board(const int xnum, const int ynum, const int num_players) : 
		xnum(xnum),ynum(ynum),data(xnum*ynum),num_players(num_players)
	{ }
	
	// ���̔Ֆʂ��R�s�[
	Board(const Board& other) : 
		xnum(other.xnum),ynum(other.ynum),data(other.data),num_players(other.num_players) 
	{ }
	
	int operator()(int x, int y) const;
	int& operator()(int x, int y);
	const std::tuple<int,int> size() const;
	int players() const;

private:
	std::size_t xnum, ynum;
	std::vector<int> data;
	const int num_players;
};

void disp(const Board& board); // �\������
int player_id(int player); // �v���C���[�ԍ�
bool placeable(const Board& board, int x, int y); // ���̏ꏊ�ɒu���邩����
int finished(const Board& board); // ����������������
int update(Board& board,const int player,
			std::function<std::tuple<int,int>(const Board&,int)> ai_function);
			// AI��\���֐�ai_function�����s���A�����ɐ΂�u��

#endif // HPP_4MOKU
