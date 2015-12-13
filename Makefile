
DBG=-g -ggdb
CXXFLAGS=-std=c++14 -Wall -Wextra

all: 4moku

4moku:source/4moku.cpp Makefile
	g++ -o 4moku source/4moku.cpp $(CXXFLAGS) $(DBG) 
