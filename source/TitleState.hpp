#pragma once
#include "ofHeader.h"
#include "State.hpp"
#include "GameSettings.hpp"
#include "Layout.hpp"
#include "AiLogic.hpp"
#include "AiList.hpp"

class TitleState : public State {
public:
	TitleState(std::shared_ptr<GameSettings> settings);
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
	VerticalLayout layout;
	AiList ai_list;
	std::vector<int> selected_ai;
};
