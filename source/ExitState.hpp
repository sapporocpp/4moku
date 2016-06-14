#pragma once
#include "ofHeader.h"
#include "State.hpp"
#include "GameSettings.hpp"

class ExitState : public State {
public:
	ExitState(std::shared_ptr<GameSettings> settings);
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
};

