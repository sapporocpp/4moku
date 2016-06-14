#pragma once
#include "GameSettings.hpp"

class State {
public:
	virtual void draw() = 0;
	virtual void setup() = 0;
	virtual void update() = 0;
};
