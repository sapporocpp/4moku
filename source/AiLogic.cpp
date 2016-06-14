#include <algorithm>
#include "AiLogic.hpp"

LogicWrapper::LogicWrapper(Function func, std::string description) :
	function(func), description(description)
{ }

std::tuple<int,int> LogicWrapper::next(const Board& board, int player) {
	return function(board, player);
}

std::string LogicWrapper::name() const {
	return description;
}

