#include "GameState.hpp"
#include "Manual.hpp"

GameState::GameState(std::shared_ptr<GameSettings> settings) :
	settings(settings), board(0,0,0)
{ 
}

void GameState::setup() {
	const int num_players = settings->get_num_players();
	font.load("verdana.ttf", 30);
	ofRegisterMouseEvents(this);

	const auto board_size = settings->getBoardSize();
	const auto xnum = std::get<0>(board_size);
	const auto ynum = std::get<1>(board_size);
	board.reset(xnum,ynum,num_players);

	stone_colors.resize(num_players);
	for(auto& color: stone_colors)
		color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));

	current_player = 0;
	result.clear();
}

void GameState::draw() {
	const int xnum = board.size_x();
	const int ynum = board.size_y();

	const int win_height = ofGetHeight();
	const int win_width  = ofGetWidth();

	//layout
	const auto grid_size = [=](){
		const int gx = win_width /xnum;
		const int gy = win_height/ynum;
		return std::min(gx,gy)*0.8; }();
	margin = {
		static_cast<int>(win_height-grid_size*ynum)/2,
		static_cast<int>(win_width -grid_size*xnum)/2,
		static_cast<int>(win_width -grid_size*xnum)/2,
		static_cast<int>(win_height-grid_size*ynum)/2,
	};

	ofBackground(150,150,200);
	const int linewidth = 4;

	// draw board
	ofSetColor(255, 224, 192);
	ofFill();
	ofDrawRectangle(margin.left, margin.top, grid_size*xnum, grid_size*ynum);
	ofSetColor(0,0,0);
	ofSetLineWidth(linewidth);
	for(int i=0;i<ynum+1;++i) {
		const auto y = margin.top+i*grid_size;
		ofDrawLine(margin.left, y, win_width-margin.right, y);
	}
	for(int i=0;i<xnum+1;++i) {
		const auto x = margin.left+i*grid_size;
		ofDrawLine(x, margin.top, x, win_height-margin.bottom);
	}

	// draw stone
	for(int i=0;i<ynum;++i) {
		for(int j=0;j<xnum;++j) {
			const int id = board(j,ynum-i-1);
			if(id==0) continue;
			ofSetColor(stone_colors[id-1]);
			const int left = margin.left+j*grid_size + linewidth/2;
			const int top  = margin.top +i*grid_size + linewidth/2;
			ofDrawRectangle(left,top,grid_size-linewidth, grid_size-linewidth);
		}
	}
	
	if(result.empty()) {
		// draw for ai
		settings->get_player(current_player)->draw();
	} else {
		ofSetColor(220,220,220);
		font.drawString(result, 10, 40);
	}
}

void GameState::update() {
	if(result.empty()) {
		const int xnum = board.size_x();
		const int ynum = board.size_y();
		auto player = settings->get_player(current_player);
		auto ret = -1;
		if(player->name()=="Manual") {
			auto mp = std::dynamic_pointer_cast<Manual>(player);
			if(!mp->isActive()) {
				mp->activate();
				mp->setBoard(board);
			}
			mp->setLayout(margin, xnum, ynum);
			if(mp->isSelected()) {
				auto func = [&](const Board& b,int p){return player->next(b,p);};
				ret = ::update(board, current_player,func);
			}
		}
		else {
			ofResetElapsedTimeCounter();
			auto func = [&](const Board& b,int p){return player->next(b,p);};
			ret = ::update(board, current_player,func);
			auto elapsed = ofGetElapsedTimeMillis();
			if(elapsed<1000) {
				ofSleepMillis(1000-elapsed);
			}
		}

		if(ret==WIN) {
			result = "Player " + std::to_string(current_player) +" WIN!";
		}
		else if(ret==FAILED) {
			result = "Player " + std::to_string(current_player) +" Lose!";
		}
		else if(ret==0) {
			current_player = (current_player+1)%settings->get_num_players();
		}
	}
}

void GameState::mouseMoved(ofMouseEventArgs& args) {
	mousePos = ofVec2f(args.x, args.y);
}

void GameState::mouseReleased(ofMouseEventArgs& args){}
void GameState::mouseDragged (ofMouseEventArgs& args){}
void GameState::mousePressed (ofMouseEventArgs& args){}
void GameState::mouseScrolled(ofMouseEventArgs& args){}
void GameState::mouseEntered (ofMouseEventArgs& args){}
void GameState::mouseExited  (ofMouseEventArgs& args){}

