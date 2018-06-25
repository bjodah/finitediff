CFLAGS ?= -std=c89 -Wall -Wextra -Werror -pedantic -O0 -g -ggdb -I../finitediff/include -I../external/newton_interval/include
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -Wpadded -pedantic -I../finitediff/include -fno-omit-frame-pointer -D_GLIBCXX_DEBUG
LDLIBS ?= -lm
CC ?= gcc
CXX ?= g++
CFLAGS += $(EXTRA_COMPILE_ARGS)
CXXFLAGS += $(EXTRA_COMPILE_ARGS)

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

newton_interval.o: ../external/newton_interval/src/newton_interval.c ../external/newton_interval/include/newton_interval.h
	$(CC) $(CFLAGS) -c -o $@ $<

test_finitediff_c: test_finitediff_c.c finitediff_c.o newton_interval.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
