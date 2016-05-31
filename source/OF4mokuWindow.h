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

private:
	int x_window_pos_;
	int y_window_pos_;
	float gridsize_;
	int next_player;
	Board board;
	AppStatus app_status;
	ofTrueTypeFont font;

	using FuncType = std::tuple<int, int>(const Board&, int);
	std::vector<std::function<FuncType>> ai_list;
	static const std::vector<const char *> ai_names;
	std::vector<int> player_ai_ids;

	void mousepos2gridcoord(int mouse_x, int mouse_y);
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
