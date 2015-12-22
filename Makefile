
DBG=-g -ggdb
CXXFLAGS=-std=c++14 -Wall -Wextra
#CXXFLAGS=-std=c++11 -Wall -Wextra

all: 4moku

4moku:source/4moku.cpp source/4moku.hpp Makefile
	g++ -o 4moku source/4moku.cpp $(CXXFLAGS) $(DBG) 
