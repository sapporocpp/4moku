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
	// 空の盤面を生成
	Board(const int xnum, const int ynum, const int num_players) : 
		xnum(xnum),ynum(ynum),data(xnum*ynum),num_players(num_players)
	{ }
	
	// 他の盤面をコピー
	Board(const Board& other) : 
		xnum(other.xnum),ynum(other.ynum),data(other.data),num_players(other.num_players) 
	{ }
	
	void reset(const int n_x, const int n_y, const int n_players);
	int operator()(int x, int y) const;
	int& operator()(int x, int y);
	const std::tuple<int,int> size() const;
	std::size_t size_x() const;
	std::size_t size_y() const;
	int players() const;

private:
	std::size_t xnum, ynum;
	std::vector<int> data;
	int num_players;
};

void disp(const Board& board); // 表示する
int player_id(int player); // プレイヤー番号
bool placeable(const Board& board, int x, int y); // その場所に置けるか判定
int finished(const Board& board); // 決着がついたか判定
int update(Board& board,const int player,
			std::function<std::tuple<int,int>(const Board&,int)> ai_function);
			// AIを表す関数ai_functionを実行し、そこに石を置く

#endif // HPP_4MOKU
