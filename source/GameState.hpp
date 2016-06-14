#pragma once
#include "ofHeader.h"

#include "State.hpp"
#include "GameSettings.hpp"
#include "AiLogic.hpp"

#include "4moku.hpp"
#include "Layout.hpp"

struct Margin  { int top, left, right, bottom; };

class GameState : public State {
public:
	GameState(std::shared_ptr<GameSettings> settings);
	void draw();
	void setup();
	void update();
	void mouseMoved   (ofMouseEventArgs& args);
	void mouseDragged (ofMouseEventArgs& args);
	void mousePressed (ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	void mouseScrolled(ofMouseEventArgs& args);
	void mouseEntered (ofMouseEventArgs& args);
	void mouseExited  (ofMouseEventArgs& args);

private:
	ofTrueTypeFont font;
	std::shared_ptr<GameSettings> settings;
	ofVec2f mousePos;

	Board board;
	Margin margin;
	std::vector<ofColor> stone_colors;

	int current_player;
	std::string result;
};


