CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -Wpadded -pedantic -I../include -fno-omit-frame-pointer

.PHONY: test debug clean

test: test_finitediff_templated
	./test_finitediff_templated

test_finitediff_templated: test_finitediff_templated.cpp ../include/finitediff_templated.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

