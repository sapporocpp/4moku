#pragma once
#include "ofHeader.h"
#include <memory>
#include <map>
#include "State.hpp"

class OFGameApp : public ofBaseApp{
public:

private:
	using AppState = GameSettings::AppState;
	std::shared_ptr<GameSettings> settings;
	std::map<AppState, std::shared_ptr<State>> state;

public:
	OFGameApp();
	void setup();
	void update();
	void draw();
	void resetState(const GameSettings::AppState& state);

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
