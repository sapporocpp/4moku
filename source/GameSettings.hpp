#pragma once
#include <vector>
#include <string>
#include "ofHeader.h"
#include "AiLogic.hpp"

class GameSettings {
public:
	enum class AppState {
		Title,
		Game,
		Exit,
	};
	ofEvent<const AppState> resetStateEvent;

	GameSettings();

	void setState(const AppState state, const bool reset);
	AppState getState() const;
	bool state_reset() const;

	int get_num_players() const;
	void set_player(const int player, const std::shared_ptr<AiLogic> ai);
	std::shared_ptr<AiLogic> get_player(const int player) const;

	std::tuple<int,int> getBoardSize() const;

private:
	int num_players;
	std::vector<std::shared_ptr<AiLogic>> players;
	AppState current_state;
	bool reset_flag;

	int board_xnum, board_ynum;
};
