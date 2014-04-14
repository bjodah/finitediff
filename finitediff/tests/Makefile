FC=gfortran
FLAGS=-Wall -Wextra -pedantic
FFLAGS=-std=f2008
CXX=g++
CXXFLAGS=-std=c++0x
.PHONY: tests

tests: test_finitediff.py test_fornberg test_c_fornberg
	@./test_fornberg
	@./test_c_fornberg
	@env PYTHONPATH=${CURDIR}/../..:${PYTHONPATH} python test_finitediff.py

test_fornberg: ../fornberg.f90 test_fornberg.f90
	$(FC) $(FLAGS) $(FFLAGS) -o $@ $^

%.o: ../%.f90
	$(FC) $(FLAGS) $(FFLAGS) -o $@ -c $^

test_c_fornberg: c_fornberg.o fornberg.o test_c_fornberg.cpp
	$(CXX) $(FLAGS) $(CXXFLAGS) -I.. -o $@ $^ -lgfortranbegin -lgfortran -lm
