#pragma once
#include "ofHeader.h"
#include "GameState.hpp"

class Manual : public AiLogic {
public:
	using Position = AiLogic::Position;
	Manual();
	Position next(const Board& board, int player);
	std::string name() const;
	void setLayout(const Margin m, const int board_xnum, const int board_ynum);
	void setBoard(const Board& board);
	bool isSelected() const;

	bool isActive() const;
	void activate();
	void deactivate();

	void draw();
	void mouseMoved   (ofMouseEventArgs& args);
	void mouseDragged (ofMouseEventArgs& args);
	void mousePressed (ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	void mouseScrolled(ofMouseEventArgs& args);
	void mouseEntered (ofMouseEventArgs& args);
	void mouseExited  (ofMouseEventArgs& args);

private:
	bool selected;
	bool active;
	Board board;
	bool mouse_event_register;
	ofVec2f mousePos;
	Margin margin;
	Position selection;
	int xnum, ynum;
};

