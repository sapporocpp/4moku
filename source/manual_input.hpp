#include "4moku.hpp"
#include <cstdlib>

struct ManualInput{
	std::tuple<int, int> parse_input(const char * buf) const{
		int x, y;
		char *x_end, *y_end;
		
		x = strtol(buf, &x_end, 10);
		if(*x_end != ',') return std::make_tuple(-1, -1);
		
		y = strtol(x_end+1, &y_end, 10);
		if(*y_end != '\0') return std::make_tuple(-1, -1);
		
		return std::make_tuple(x, y);
	}
	
	// AI‚Ì“à—e
	std::tuple<int, int> ai(const Board& board, int player) const{
		std::string input;
		std::tuple<int, int> result;
		
		for(;;){
			std::cout << "[Player " << player << ": Input where to put by the format \"x,y\".]" << std::endl;
			std::getline(std::cin, input);
			result = parse_input(input.c_str());
			if(std::get<0>(result) == -1){
				std::cout << "[Invalid input format.]" << std::endl;
				continue;
			}
			if(!placeable(board, std::get<0>(result), std::get<1>(result))){
				std::cout << "[Cannot be put at the place.]" << std::endl;
				continue;
			}
			return result;
		}
	}
};
