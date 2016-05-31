#include "OF4mokuWindow.h"

// ここに使うAIを定義する
#include "test_ai.hpp"
#include "random_ai2.hpp"
#include "ai_winning.hpp"
#include "kakutei.hpp"
#include "ignis-neighbors.hpp"
#include "kakutei-more.hpp"

//--------------------------------------------------------------
// 補助関数
//--------------------------------------------------------------

// マウスの座標をグリッドの座標として取得
void OF4mokuWindow::mousepos2gridcoord(int mouse_x, int mouse_y) {
	x_window_pos_ = mouse_x / gridsize_;
	y_window_pos_ = mouse_y / gridsize_;
}


//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
OF4mokuWindow::OF4mokuWindow()
	: x_window_pos_(-1), y_window_pos_(-1), gridsize_(0), next_player(0),
	board(10, 5, 2), app_status(AppStatus::Title), player_ai_ids(2, -1) {
	
	// 対局AIを準備
	using FuncType = std::tuple<int, int>(const Board&, int);

	TestAI test_ai;
	KakuteiMore<3> kakutei_more;

	ai_list = {
		[&](const Board& board, int player) { return test_ai(board, player); },
		[&](const Board& board, int player) { return AI_Neighbors(board, player); },
		[&](const Board& board, int player) { return kakutei_more.ai(board, player); },
		[&](const Board& board, int player) { return random_ai2(board, player); }
	};
}
const std::vector<const char *> OF4mokuWindow::ai_names = {
	"AI-Random",
	"AI-Neighbors (by ignis)",
	"AI-FixedSimulation (by H.Hiro)",
	"AI-RandomSimulation (by ignis)"
};

//--------------------------------------------------------------
// ウィンドウが立ち上がったときの処理
//--------------------------------------------------------------
void OF4mokuWindow::setup() {

}

//--------------------------------------------------------------
// 画面描画前の処理
//--------------------------------------------------------------
void OF4mokuWindow::update(){
	float window_width = static_cast<float>(ofGetWidth());
	float window_height = static_cast<float>(ofGetHeight());

	int new_gridsize = std::min(
		window_width / board.size_x(),
		window_height / (board.size_y() + board.players() + 1)
	);

	if (new_gridsize != gridsize_) {
		gridsize_ = new_gridsize;
	}
}

//--------------------------------------------------------------
// タイトル画面の描画
//--------------------------------------------------------------

void OF4mokuWindow::draw_for_title() {
	ofFill();
	for (int y = 0; y < 4; ++y) {
		if (y == y_window_pos_) {
			ofSetColor(192, 255, 192);
		} else {
			ofSetColor(224, 224, 224);
		}
		ofDrawRectangle(0, y * gridsize_, static_cast<float>(ofGetWidth()), gridsize_);
	}
	ofSetColor(0, 0, 0);
	ofDrawBitmapString("4moku", 0, gridsize_);
	ofDrawBitmapString("Player 1:", 0, 2 * gridsize_);
	ofDrawBitmapString((player_ai_ids[0] == -1 ? "Manual" : ai_names[player_ai_ids[0]]), 3 * gridsize_, 2 * gridsize_);
	ofDrawBitmapString("Player 2:", 0, 3 * gridsize_);
	ofDrawBitmapString((player_ai_ids[1] == -1 ? "Manual" : ai_names[player_ai_ids[1]]), 3 * gridsize_, 3 * gridsize_);
	ofDrawBitmapString("Start", 0, 4 * gridsize_);
}

//--------------------------------------------------------------
// ゲーム中の描画
//--------------------------------------------------------------

// 背景の1マスを描画
void OF4mokuWindow::draw_grid(int x, int y, bool has_cursor, bool is_placeable) {
	float grid_corner_x = x * gridsize_;
	float grid_corner_y = (board.size_y() - 1 - y) * gridsize_;

	ofColor bgcolor;
	if (has_cursor) {
		if (is_placeable) {
			// カーソルあり & 置ける
			ofSetColor(192, 255, 192);
		} else {
			// カーソルあり & 置けない
			ofSetColor(128, 128, 128);
		}
	} else {
		// 普通の背景色
		ofSetColor(255, 224, 192);
	}
	ofFill();
	ofDrawRectangle(grid_corner_x, grid_corner_y, gridsize_, gridsize_);
	ofSetColor(0, 0, 0);
	ofNoFill();
	ofDrawRectangle(grid_corner_x, grid_corner_y, gridsize_, gridsize_);
}

// 石を描画
void OF4mokuWindow::draw_stone(int x, int y, int player) {
	// 石がない場合は何も描画しない
	if (player == 0) return;

	float stone_size = gridsize_ * 0.96;
	float stone_center_x = x * gridsize_ + gridsize_ * 0.5;
	float stone_center_y = (board.size_y() - 1 - y) * gridsize_ + gridsize_ * 0.5;

	if (player == 1) {
		ofSetColor(0, 0, 0);
	}
	else {
		ofSetColor(255, 255, 255);
	}
	ofFill();
	ofDrawEllipse(stone_center_x, stone_center_y, stone_size, stone_size);

	ofSetColor(0, 0, 0);
	ofNoFill();
	ofDrawEllipse(stone_center_x, stone_center_y, stone_size, stone_size);
}

// 全体
void OF4mokuWindow::draw_for_game() {
	for (int x = 0; x < board.size_x(); ++x) {
		for (int y = 0; y < board.size_y(); ++y) {
			int y_board_pos = board.size_y() - 1 - y_window_pos_;

			draw_grid(x, y, (x == x_window_pos_ && y == y_board_pos), placeable(board, x, y));

			// 石を描画
			draw_stone(x, y, board(x, y));
		}
	}
}

//--------------------------------------------------------------
// 描画イベント
//--------------------------------------------------------------
void OF4mokuWindow::draw(){
	switch (app_status) {
	case AppStatus::Title:
		draw_for_title();
		break;
	case AppStatus::Game:
		draw_for_game();
		break;
	}
}

//--------------------------------------------------------------
// タイトル画面のマウスクリック
//--------------------------------------------------------------
void OF4mokuWindow::click_for_title(int button) {
	if (button == OF_MOUSE_BUTTON_LEFT) {
		switch (y_window_pos_) {
		case 1: case 2:
			{
				int player_id = y_window_pos_ - 1;
				++player_ai_ids[player_id];
				if (player_ai_ids[player_id] >= ai_list.size()) player_ai_ids[player_id] = -1;
			}
			break;
		case 3:
			// ゲーム開始
			next_player = 0;
			board.reset(10, 5, 2);

			app_status = AppStatus::Game;
			break;
		default:
			return;
		}
		draw();
	}
}

//--------------------------------------------------------------
// ゲーム中ののマウスクリック
//--------------------------------------------------------------
void OF4mokuWindow::click_for_game(int button) {
	if (button == OF_MOUSE_BUTTON_LEFT) {
		int y_board_pos = board.size_y() - 1 - y_window_pos_;
		if (placeable(board, x_window_pos_, y_board_pos)) {
			board(x_window_pos_, y_board_pos) = player_id(next_player);
			++next_player;
			if (next_player >= board.players()) next_player = 0;
			draw();
		}
	}
}

//--------------------------------------------------------------
// マウスイベント
//--------------------------------------------------------------
void OF4mokuWindow::mouseMoved(int x, int y) {
	mousepos2gridcoord(x, y);
	draw();
}

void OF4mokuWindow::mousePressed(int x, int y, int button) {
	mousepos2gridcoord(x, y);
	switch (app_status) {
	case AppStatus::Title:
		click_for_title(button);
		break;
	case AppStatus::Game:
		click_for_game(button);
		break;
	}
}

void OF4mokuWindow::mouseDragged(int x, int y, int button) {
	// do nothing
}

void OF4mokuWindow::mouseReleased(int x, int y, int button) {
	// do nothing
}

void OF4mokuWindow::mouseEntered(int x, int y) {
	// do nothing
}

void OF4mokuWindow::mouseExited(int x, int y) {
	// do nothing
}

//--------------------------------------------------------------
// その他、使ってないもの
//--------------------------------------------------------------
void OF4mokuWindow::keyPressed(int key){}
void OF4mokuWindow::keyReleased(int key){}
void OF4mokuWindow::windowResized(int w, int h){}
void OF4mokuWindow::gotMessage(ofMessage msg){}
void OF4mokuWindow::dragEvent(ofDragInfo dragInfo){}
