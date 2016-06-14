#include "TitleState.hpp"

TitleState::TitleState(std::shared_ptr<GameSettings> settings) :
	settings(settings)
{ }

void TitleState::setup() {
	const int num_players = settings->get_num_players();

	font.load("verdana.ttf", 30);
	ofRegisterMouseEvents(this);

	layout.clear();
	layout.push(10); // Grid1
	for(int i=0;i<num_players;++i)
		layout.push(8); // Grid2
	layout.push(10); // Grid3

	selected_ai = decltype(selected_ai)(num_players, 0);
}

void TitleState::draw() {
	/*
	 * -------------------
	 *  (Grid1) Sappro.cpp 4moku Game!!
	 *    (Grid2) player1  
	 *    (Grid2) player2  
	 *    .
	 *    .
	 *  (Grid3) Game Start
	 * -------------------
	 */
	const int num_players = settings->get_num_players();
	const int offset = ofGetHeight()*(layout.bottom(0)-layout.top(0))/4;
	ofBackground(50,50,50);

	// Grid1 
	ofSetColor(200,250,250);
	const int grid1bottom = ofGetHeight()*layout.bottom(0);
	font.drawString("Sapporo.cpp 4moku Game!!", 10, grid1bottom-offset);

	// Grid2
	for(int i=0;i<num_players;++i) {
		std::string str = "Player ";
		str += std::to_string(i);
		str += ": " + ai_list(selected_ai[i])->name();
		const int grid2top    = ofGetHeight()*layout.top(1+i);
		const int grid2bottom = ofGetHeight()*layout.bottom(1+i);
		if(grid2top < mousePos.y && mousePos.y < grid2bottom){
			ofSetColor(50,150,150);
			ofDrawRectangle(0, grid2top, ofGetWidth(), grid2bottom-grid2top);
		}
		ofSetColor(200,250,250);
		font.drawString(str, 20, grid2bottom - offset);
	}

	// Grid3
	const int top    = ofGetHeight()*layout.top(1+num_players);
	const int bottom = ofGetHeight()*layout.bottom(1+num_players);
	if(top < mousePos.y && mousePos.y < bottom){
		ofSetColor(50,150,150);
		ofDrawRectangle(0, top, ofGetWidth(), bottom-top);
	}
	ofSetColor(200,250,250);
	font.drawString("Game Start", 30, bottom-offset);
}

void TitleState::mouseMoved(ofMouseEventArgs& args) {
	mousePos = ofVec2f(args.x, args.y);
}

void TitleState::mouseReleased(ofMouseEventArgs& args){
	const int num_players = settings->get_num_players();

	for(int i=0;i<num_players;++i) 
	{
		const int grid2id = 1 + i;
		const int top    = ofGetHeight()*layout.top(grid2id);
		const int bottom = ofGetHeight()*layout.bottom(grid2id);
		if(top < mousePos.y && mousePos.y < bottom){
			selected_ai[i] = (selected_ai[i]+1)%ai_list.count();
		}
	}

	{
		const int grid3id = 1 + settings->get_num_players();
		const int top    = ofGetHeight()*layout.top(grid3id);
		const int bottom = ofGetHeight()*layout.bottom(grid3id);
		if(top < mousePos.y && mousePos.y < bottom){
			// GameState�ֈڍs
			for(int i=0;i<num_players;++i) {
				settings->set_player(i, ai_list(selected_ai[i]));
			}
			ofUnregisterMouseEvents(this);
			settings->setState(GameSettings::AppState::Game, true);
		}
	}
}

void TitleState::update() {}
void TitleState::mouseDragged (ofMouseEventArgs& args){}
void TitleState::mousePressed (ofMouseEventArgs& args){}
void TitleState::mouseScrolled(ofMouseEventArgs& args){}
void TitleState::mouseEntered (ofMouseEventArgs& args){}
void TitleState::mouseExited  (ofMouseEventArgs& args){}

