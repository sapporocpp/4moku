#include "OF4mokuWindow.h"
#include <boost/filesystem.hpp>

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

int OF4mokuWindow::mouse_gridX_board() const {
	return mouse_gridX_;
}

int OF4mokuWindow::mouse_gridY_board() const {
	return board.size_y() - mouse_gridY_ - 1;
}

// マウスの座標をグリッドの番号に変換
void OF4mokuWindow::mousepos2gridcoord(int mouse_x, int mouse_y) {
	mouse_gridX_ = mouse_x / gridsize_;
	mouse_gridY_ = mouse_y / gridsize_;
}

// アプリの状態を変更
void OF4mokuWindow::update_app_status(AppStatus new_status) {
	app_status = new_status;
	windowResized(ofGetWidth(), ofGetHeight());
}

// AIの手を進める。
// AIではないプレイヤーの番が来たか、ゲームが決着した時点で止める。
// 止めた後は描画も行う。
void OF4mokuWindow::progress_game() {
	for (;;) {
		if (game_winner) break;

		if (player_ai_ids[next_player] < 0) {
			// AIではないプレイヤーの場合
			break;
		} else {
			// AIの場合
			int x, y;
			std::tie(x, y) = (ai_list[player_ai_ids[next_player]])(board, next_player);

			last_move_x = x;
			last_move_y = y;
			board(x, y) = player_id(next_player);
			++next_player;
			if (next_player >= board.players()) next_player = 0;
			game_winner = finished(board);
		}
	}
	draw();
}

// 文字列を描画
void OF4mokuWindow::draw_str(const std::string & buf, int x, int y) {
	if (font.isLoaded()) {
		font.drawString(buf, x, y);
	}else{
		ofDrawBitmapString(buf, x, y);
	}
}

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
OF4mokuWindow::OF4mokuWindow()
	: mouse_gridX_(-1), mouse_gridY_(-1), gridsize_(0), next_player(0),
	board(10, 5, NUM_PLAYERS), game_winner(0), player_ai_ids(NUM_PLAYERS, -1) {

	// フォントのファイル名
	font_path = ofFilePath::join(ofFilePath::getCurrentExeDir(), "Inconsolata-Regular.ttf");

	// アプリの状態
	update_app_status(AppStatus::Title);

	// 対局AIを準備
	using FuncType = std::tuple<int, int>(const Board&, int);

	TestAI test_ai;
	KakuteiMore<2> kakutei_more;

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
	"AI-FixedTurnSimulation (by H.Hiro)",
	"AI-RandomSimulation (by ignis)"
};

//--------------------------------------------------------------
// ウィンドウが立ち上がったときの処理
//--------------------------------------------------------------
void OF4mokuWindow::setup() {

}

//--------------------------------------------------------------
// ウィンドウサイズが変更されたときの処理
//--------------------------------------------------------------
void OF4mokuWindow::windowResized(int w, int h) {
	float new_gridsize;
	switch (app_status) {
	case AppStatus::Title:
		new_gridsize = h / (board.players() + ROWS_TITLEINFO);
		break;
	case AppStatus::Game:
		new_gridsize = std::min(
			w / board.size_x(),
			h / (board.size_y() + board.players() + ROWS_GAMEINFO)
		);
		break;
	default:
		break;
	}

	if (gridsize_ != new_gridsize) {
		// 画面サイズが変更された場合
		gridsize_ = new_gridsize;
		font.loadFont(font_path, std::min(static_cast<int>(gridsize_), w / CHARS_PER_WINDOW_WIDTH));
	}
	draw();
}

//--------------------------------------------------------------
// 画面描画前の処理
//--------------------------------------------------------------
void OF4mokuWindow::update(){
}

//--------------------------------------------------------------
// タイトル画面の描画
//--------------------------------------------------------------

void OF4mokuWindow::draw_for_title() {
	ofFill();
	for (int y = 0; y < NUM_PLAYERS + 2; ++y) {
		if (y > 0 && y == mouse_gridY_) {
			ofSetColor(192, 255, 192);
		} else {
			ofSetColor(224, 224, 224);
		}
		ofDrawRectangle(0, y * gridsize_, static_cast<float>(ofGetWidth()), gridsize_);
	}
	ofSetColor(0, 0, 0);
	draw_str("4moku", 0, gridsize_);

	std::stringstream buf;

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		buf.str("");
		buf << "Player " << (i+1) << ": " << (player_ai_ids[i] == -1 ? "Manual" : ai_names[player_ai_ids[i]]);
		draw_str(buf.str(), 0, (i + 2) * gridsize_);
	}
	draw_str("Start Game", 0, (NUM_PLAYERS + 2) * gridsize_);
}

//--------------------------------------------------------------
// ゲーム中の描画
//--------------------------------------------------------------

// 背景の1マスを描画
void OF4mokuWindow::draw_grid(int x, int y, bool has_cursor, bool is_placeable) {
	float grid_corner_x = x * gridsize_;
	float grid_corner_y = (board.size_y() - 1 - y) * gridsize_;
	ofSetLineWidth(1);

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
	ofSetLineWidth(1);

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
	int y_board_pos = board.size_y() - 1 - mouse_gridY_;

	// 盤面
	for (int x = 0; x < board.size_x(); ++x) {
		for (int y = 0; y < board.size_y(); ++y) {
			// マスを描画
			draw_grid(x, y, (game_winner == 0 && x == mouse_gridX_ && y == y_board_pos), placeable(board, x, y));

			// 石を描画
			draw_stone(x, y, board(x, y));
		}
	}
	if (last_move_x >= 0 && last_move_y >= 0) {
		// 最後に指された手
		ofSetColor(255, 0, 0);
		ofNoFill();
		ofSetLineWidth(3);
		ofDrawRectangle(last_move_x * gridsize_, (board.size_y() - 1 - last_move_y) * gridsize_, gridsize_, gridsize_);
	}

	// 対局状況
	std::stringstream buf;

	// 次に指すプレイヤー、決着など
	if (game_winner && mouse_gridY_ == board.size_y()) {
		ofSetColor(192, 255, 192);
	}
	else {
		ofSetColor(255, 255, 255);
	}
	ofFill();
	ofDrawRectangle(0, board.size_y() * gridsize_, static_cast<float>(ofGetWidth()), gridsize_);
	if (game_winner) {
		buf.str("");
		buf << "Player " << game_winner << " won the game! Click to show title";
	} else {
		buf.str("");
		buf << "Turn for Player " << player_id(next_player) << ".";
		draw_stone(board.size_x() - 1, -1, player_id(next_player));
	}
	ofSetColor(0, 0, 0);
	draw_str(buf.str(), 0, (board.size_y() + 1) * gridsize_);
	
	// 各プレイヤーが何のAIであるか
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		buf.str("");
		buf << "Player" << player_id(i) << ": ";
		if (player_ai_ids[i] >= 0) {
			buf << ai_names[player_ai_ids[i]];
		}
		else {
			buf << "Manual";
		}
		ofSetColor(255, 255, 255);
		ofFill();
		ofDrawRectangle(0, (board.size_y() + i + 1) * gridsize_, static_cast<float>(ofGetWidth()), gridsize_);
		ofSetColor(0, 0, 0);
		draw_str(buf.str(), 0, (board.size_y() + i + 2) * gridsize_);
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
	if (mouse_gridY_ >= 1 && mouse_gridY_ <= NUM_PLAYERS) {
		int player = mouse_gridY_ - 1;

		switch (button) {
		case OF_MOUSE_BUTTON_LEFT:
			++player_ai_ids[player];
			if (player_ai_ids[player] >= ai_list.size()) player_ai_ids[player] = -1;
			break;
		case OF_MOUSE_BUTTON_RIGHT:
			--player_ai_ids[player];
			if (player_ai_ids[player] < -1) player_ai_ids[player] = ai_list.size() -1;
			break;
		}
	} else if (mouse_gridY_ == NUM_PLAYERS + 1) {
		if (button == OF_MOUSE_BUTTON_LEFT) {
			// ゲーム開始
			next_player = 0;
			game_winner = 0;
			board.reset(10, 5, 2);
			last_move_x = -1;
			last_move_y = -1;

			update_app_status(AppStatus::Game);
			progress_game();
		}
	}
	draw();
}

//--------------------------------------------------------------
// ゲーム中のマウスクリック
//--------------------------------------------------------------
void OF4mokuWindow::click_for_game(int button) {
	if (game_winner) {
		// 決着が着いた後
		if (button == OF_MOUSE_BUTTON_LEFT && mouse_gridY_ == board.size_y()) {
			update_app_status(AppStatus::Title);
			draw();
		}
	}else {
		// ゲーム進行中
		if (button == OF_MOUSE_BUTTON_LEFT) {
			if (placeable(board, mouse_gridX_board(), mouse_gridY_board())) {
				// 石を置く
				last_move_x = mouse_gridX_board();
				last_move_y = mouse_gridY_board();

				// TODO: 以下の部分はprogress_gameと同じ処理なのでリファクタリングする
				board(mouse_gridX_board(), mouse_gridY_board()) = player_id(next_player);
				++next_player;
				if (next_player >= board.players()) next_player = 0;
				game_winner = finished(board);

				progress_game();
				draw();
			}
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
void OF4mokuWindow::gotMessage(ofMessage msg){}
void OF4mokuWindow::dragEvent(ofDragInfo dragInfo){}
