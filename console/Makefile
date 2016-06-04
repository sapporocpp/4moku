
#DBG=-g -ggdb
DBG=
#CXXFLAGS=-std=c++14 -Wall -Wextra
CXXFLAGS=-std=c++11 -Wall -Wextra -I../source

all: 4moku-console

../source/4moku.cpp: ../source/4moku.hpp

.cpp.o:
	g++ -c $< $(CXXFLAGS)

4moku-console: 4moku-console.o ../source/4moku.o Makefile
	g++ -o 4moku-console 4moku-console.o 4moku.o $(CXXFLAGS) $(DBG) 

clean:
	rm -rf *.o
