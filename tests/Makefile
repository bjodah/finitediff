CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -Wpadded -pedantic -I../finitediff/include -fno-omit-frame-pointer -D_GLIBCXX_DEBUG

.PHONY: test debug clean

test: test_finitediff_templated
	./test_finitediff_templated

catch.hpp: catch.hpp.bz2
	bunzip2 -k -f $<

test_finitediff_templated: test_finitediff_templated.cpp ../finitediff/include/finitediff_templated.hpp catch.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

