INCLUDE?=-I../include
CXXFLAGS?=-std=c++11 -Wall -Werror -pedantic -Wextra -g -O0 -D_GLIBCXX_DEBUG

.PHONY: all

all: demo demo_ordering
	./demo
	./demo_ordering

%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $<

demo_ord: demo_ordering.cpp
	clang++-3.8 -I../include -std=c++11 -fsanitize=address -g -O1 -o $@ $<
	ASAN_SYMBOLIZER_PATH=/usr/lib/llvm-3.8/bin/llvm-symbolizer ASAN_OPTIONS=symbolize=1 ./demo_ord
