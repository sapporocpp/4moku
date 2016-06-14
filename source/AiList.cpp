#include "AiList.hpp"
#include "4moku.hpp"

// ここに使うAIを定義する
#include "test_ai.hpp"
#include "random_ai2.hpp"
#include "ai_winning.hpp"
#include "kakutei.hpp"
#include "ignis-neighbors.hpp"
#include "kakutei-more.hpp"

AiList::AiList() {
	logics = {
		std::make_shared<LogicWrapper>(AI_Neighbors, "Test AI"),
		std::make_shared<LogicWrapper>(random_ai2,   "Random2"),
	};
}

int AiList::count() const {
	return logics.size();
}

std::vector<std::string> AiList::names() const {
	std::vector<std::string> names(logics.size());
	std::transform(logics.begin(), logics.end(),
					names.begin(), [](auto ai){return ai->name();});
	return names;
}

std::shared_ptr<AiLogic> AiList::operator()(const int i) const {
	return logics[i];
}

