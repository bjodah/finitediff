FC=gfortran
FLAGS=-Wall -Wextra -pedantic
FFLAGS=-std=f2008
CXX=g++
CXXFLAGS=-std=c++0x
.PHONY: tests

tests: test_finitediff_fort test_finitediff
	./test_finitediff_fort
	./test_finitediff

test_finitediff_fort: ../finitediff_fort.f90 test_finitediff_fort.f90
	$(FC) $(FLAGS) $(FFLAGS) -o $@ $^

%.o: ../%.f90
	$(FC) $(FLAGS) $(FFLAGS) -o $@ -c $^

test_finitediff: c_finitediff_fort.o finitediff_fort.o test_finitediff.cpp ../../include/finitediff_templated.hpp
	$(CXX) $(FLAGS) $(CXXFLAGS) -I../../include -o $@ $^ -lgfortranbegin -lgfortran -lm
