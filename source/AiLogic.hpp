#pragma once
#include<string>
#include<vector>
#include<memory>
#include"ofHeader.h"

class Board;
class AiLogic {
public:
	using Position = std::tuple<int,int>;

	virtual Position next(const Board& board, int player) = 0;
	virtual std::string name() const = 0;
	virtual void draw() { };

	virtual void mouseMoved   (ofMouseEventArgs& args){};
	virtual void mouseDragged (ofMouseEventArgs& args){};
	virtual void mousePressed (ofMouseEventArgs& args){};
	virtual void mouseReleased(ofMouseEventArgs& args){};
	virtual void mouseScrolled(ofMouseEventArgs& args){};
	virtual void mouseEntered (ofMouseEventArgs& args){};
	virtual void mouseExited  (ofMouseEventArgs& args){};
};

class LogicWrapper : public AiLogic {
public:
	using Function = std::function<Position(const Board&, int)>;

	LogicWrapper(Function func, std::string description);
	Position next(const Board& board, int player);
	std::string name() const;

private:
	Function function;
	std::string description;
};


