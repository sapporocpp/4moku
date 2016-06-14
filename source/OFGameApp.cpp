#include "OFGameApp.hpp"
#include "GameSettings.hpp"
#include "TitleState.hpp"
#include "GameState.hpp"
#include "ExitState.hpp"

OFGameApp::OFGameApp() {
}

void OFGameApp::setup() {
	settings = std::make_shared<GameSettings>();

	state[AppState::Title] = std::make_shared<TitleState>(settings);
	state[AppState::Game] = std::make_shared<GameState>(settings);
	state[AppState::Exit] = std::make_shared<ExitState>(settings);

	ofAddListener(settings->resetStateEvent, this, &OFGameApp::resetState);
	settings->setState(AppState::Title, true);
}

void OFGameApp::update() {
	state[settings->getState()]->update();
}

void OFGameApp::resetState(const GameSettings::AppState& s) {
	state[s]->setup();
}

void OFGameApp::draw() {
	state[settings->getState()]->draw();
}

void OFGameApp::keyPressed(int key) {}
void OFGameApp::keyReleased(int key) {}
void OFGameApp::mouseMoved(int x, int y ) {}
void OFGameApp::mouseDragged(int x, int y, int button) {}
void OFGameApp::mousePressed(int x, int y, int button) {}
void OFGameApp::mouseReleased(int x, int y, int button) {}
void OFGameApp::mouseEntered(int x, int y) {}
void OFGameApp::mouseExited(int x, int y) {}
void OFGameApp::windowResized(int w, int h) {}
void OFGameApp::dragEvent(ofDragInfo dragInfo) {}
void OFGameApp::gotMessage(ofMessage msg) {}
