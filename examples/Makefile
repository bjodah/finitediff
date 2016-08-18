INCLUDE?=-I../include
CXXFLAGS?=-std=c++11 -Wall -Werror -pedantic -Wextra

.PHONY: all

all: demo
	./demo

demo: demo.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $<
