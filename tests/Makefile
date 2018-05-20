CFLAGS ?= -std=c89 -Wall -Wextra -Werror -pedantic -I../finitediff/include
LDLIBS ?= -lm
CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -Wpadded -pedantic -I../finitediff/include -fno-omit-frame-pointer -D_GLIBCXX_DEBUG

.PHONY: test debug clean

test: test_finitediff_templated test_finitediff_c
	./test_finitediff_templated
	./test_finitediff_c

catch.hpp: catch.hpp.bz2
	bunzip2 -k -f $<

test_finitediff_templated: test_finitediff_templated.cpp ../finitediff/include/finitediff_templated.hpp catch.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

finitediff_c.o: ../src/finitediff_c.c ../finitediff/include/finitediff_c.h
	$(CC) $(CFLAGS) -c -o $@ $<

test_finitediff_c: test_finitediff_c.c finitediff_c.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

