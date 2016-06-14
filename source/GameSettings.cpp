#include "GameSettings.hpp"

GameSettings::GameSettings() {
	num_players = 2;
	players.resize(num_players);

	board_xnum = 8;
	board_ynum = 8;
	reset_flag = false;
}

void GameSettings::setState(const AppState state, const bool reset) {
	current_state = state;
	ofNotifyEvent(resetStateEvent, current_state, this);
}

GameSettings::AppState GameSettings::getState() const {
	return current_state;
}

int GameSettings::get_num_players() const {
	return num_players;
}

void GameSettings::set_player(const int player, const std::shared_ptr<AiLogic> ai) {
	players[player] = ai;
}

std::shared_ptr<AiLogic> GameSettings::get_player(const int player) const {
	return players[player];
}

std::tuple<int,int> GameSettings::getBoardSize() const {
	return std::make_tuple(board_xnum, board_ynum);
}
