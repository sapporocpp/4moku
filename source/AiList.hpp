#pragma once
#include "AiLogic.hpp"

class AiList {
public:
	AiList();
	int count() const;
	std::vector<std::string> names() const;
	std::shared_ptr<AiLogic> operator()(const int i) const;

private:
	std::vector<std::shared_ptr<AiLogic>> logics;
};
