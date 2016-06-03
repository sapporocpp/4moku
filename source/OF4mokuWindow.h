#pragma once

#include "ofMain.h"
#include "4moku.hpp"
#include <algorithm>
#include <vector>

class OF4mokuWindow : public ofBaseApp{
public:
	enum class AppStatus {
		Title, Game
	};

	// 対局中におけるゲーム情報を表示する行数
	const int ROWS_GAMEINFO = 1;

	// タイトル中の情報を表示する行数
	const int ROWS_TITLEINFO = 2;

	// プレイヤー数
	const int NUM_PLAYERS = 2;

private:
	int mouse_gridX_; // マウスの位置を、グリッドの番号（一番左が0）で表したもの
	int mouse_gridY_; // マウスの位置を、グリッドの番号（一番上が0）で表したもの
	// 盤面の位置として利用する場合、
	// mouse_gridX_ はそのまま盤面のx座標としてよい一方、
	// mouse_gridY_ はy座標と逆向きになるため、そのままは利用できない。
	// 盤面の座標が必要な場合は、 mouse_gridX_board と mouse_gridY_board を用いる。
	
	float gridsize_;
	int next_player;
	int last_move_x, last_move_y; // 最後に指された手（強調表示するため）
	Board board;
	AppStatus app_status;
	int game_winner;
	ofTrueTypeFont font;

	using FuncType = std::tuple<int, int>(const Board&, int);
	std::vector<std::function<FuncType>> ai_list;
	static const std::vector<const char *> ai_names;
	std::vector<int> player_ai_ids;

	int mouse_gridX_board() const;
	int mouse_gridY_board() const;
	void mousepos2gridcoord(int mouse_x, int mouse_y);
	void progress_game();

	void update_app_status(AppStatus new_status);
	void draw_for_title(); // タイトル画面の描画
	void draw_for_game(); // ゲーム中の描画
	void click_for_title(int button); // タイトル画面のマウスクリック
	void click_for_game(int button); // ゲーム中のマウスクリック
	void draw_grid(int x, int y, bool has_cursor, bool is_placeable);
	void draw_stone(int x, int y, int player);
	// void draw_for_main_menu(); // メインメニューにおける描画


public:
	OF4mokuWindow();
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
		
};
