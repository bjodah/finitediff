CXX ?= g++
CXXFLAGS?=-std=c++11 -Wall -Werror -pedantic -Wextra -g -O0
CXXFLAGS+=$(EXTRA_CXX_FLAGS)
INCLUDE?=-I../finitediff/include
EXTRA_COMPILE_ARGS ?=

.PHONY: all

all: demo
	./demo

%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(EXTRA_COMPILE_ARGS) -o $@ $<
