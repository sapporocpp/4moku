#pragma once
#include<string>
#include<vector>
#include<memory>
#include<functional>

class Board;
class AiLogic {
public:
	using Position = std::tuple<int,int>;

	virtual Position next(const Board& board, int player) = 0;
	virtual std::string name() const = 0;
	virtual void draw() { };
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


