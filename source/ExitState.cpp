#include "ExitState.hpp"

ExitState::ExitState(std::shared_ptr<GameSettings> settings) :
	settings(settings)
{ }

void ExitState::setup() {
	font.load("verdana.ttf", 30);
	ofRegisterMouseEvents(this);
}

void ExitState::draw() { 
	ofExit();
}

void ExitState::update() {}
void ExitState::mouseMoved   (ofMouseEventArgs& args){}
void ExitState::mouseDragged (ofMouseEventArgs& args){}
void ExitState::mousePressed (ofMouseEventArgs& args){}
void ExitState::mouseReleased(ofMouseEventArgs& args){}
void ExitState::mouseScrolled(ofMouseEventArgs& args){}
void ExitState::mouseEntered (ofMouseEventArgs& args){}
void ExitState::mouseExited  (ofMouseEventArgs& args){}

