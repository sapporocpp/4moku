#include "Manual.hpp"

struct Grid { int x,y; };
Grid convert_from_mouse_pos(
	const ofVec2f mousePos, 
	const Margin margin, 
	const int board_xnum, const int board_ynum) 
{
	const auto board_width = ofGetWidth()  - margin.right -margin.left;
	const auto board_height= ofGetHeight() - margin.bottom-margin.top;
	const auto gridsize_x = board_width /board_xnum;
	const auto gridsize_y = board_height/board_ynum;
	const int grid_x = (mousePos.x-margin.left)/gridsize_x;
	const int grid_y = (mousePos.y-margin.top) /gridsize_y;
	return {grid_x, grid_y};
}

Manual::Manual() :
	selected(false),
	active(false),
	board(0,0,0)
{}

AiLogic::Position Manual::next(const Board& board, int player) {
	if(!selected) {
		throw std::runtime_error("check isSelected");
	}
	selected = false;
	deactivate();
	return selection;
}

std::string Manual::name() const {
	return "Manual";
}

void Manual::setLayout(const Margin m, const int board_xnum, const int board_ynum) {
	margin = m;
	xnum = board_xnum;
	ynum = board_ynum;
}

void Manual::setBoard(const Board& board) {
	this->board = board;
}

bool Manual::isActive() const {
	return active;
}

void Manual::activate() {
	ofRegisterMouseEvents(this);
	active = true;
}

void Manual::deactivate() {
	active = false;
	ofUnregisterMouseEvents(this);
}

bool Manual::isSelected() const {
	return selected;
}

void Manual::draw() {
	if(board.size_x()==0)
		return;
	Grid pos = convert_from_mouse_pos(mousePos,margin,xnum,ynum);
	if(placeable(board, pos.x, ynum-pos.y-1)) {
		const auto board_width = ofGetWidth() -margin.right -margin.left;
		const auto board_height= ofGetHeight()-margin.bottom-margin.top;
		const auto gridsize_x  = board_width /xnum;
		const auto gridsize_y  = board_height/ynum;
		ofSetColor(255,0,0);
		const auto left = margin.left + pos.x*gridsize_x;
		const auto top  = margin.top  + pos.y*gridsize_y;
		ofNoFill();
		ofDrawRectangle(left,top,gridsize_x,gridsize_y);
	}
}

void Manual::mouseMoved(ofMouseEventArgs& args) {
	mousePos = ofVec2f(args.x, args.y);
}

void Manual::mouseReleased(ofMouseEventArgs& args){
	Grid pos = convert_from_mouse_pos(mousePos,margin,xnum,ynum);
	if(placeable(board, pos.x, ynum-pos.y-1)) {
		selection = std::make_tuple(pos.x, ynum-pos.y-1);
		selected = true;
	}
}
void Manual::mouseDragged (ofMouseEventArgs& args){}
void Manual::mousePressed (ofMouseEventArgs& args){}
void Manual::mouseScrolled(ofMouseEventArgs& args){}
void Manual::mouseEntered (ofMouseEventArgs& args){}
void Manual::mouseExited  (ofMouseEventArgs& args){}
