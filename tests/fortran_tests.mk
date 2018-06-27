CXX ?= g++
FC ?=gfortran
FLAGS ?= -Wall -Wextra -pedantic

.PHONY: test

test: test_finitediff_fort test_finitediff
	./test_finitediff_fort
	./test_finitediff

test_finitediff_fort: ../src/finitediff_fort.f90 test_finitediff_fort.f90
	$(FC) $(FLAGS) -o $@ $^

%.o: ../src/%.f90
	$(FC) $(FLAGS) -o $@ -c $^

test_finitediff: c_finitediff_fort.o finitediff_fort.o test_finitediff.cpp
	$(CXX) $(FLAGS) -std=c++11 -I../finitediff/include -o $@ $^ -lgfortran -lm
