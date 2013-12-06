FC=gfortran
FFLAGS=-std=f2008
.PHONY: tests

tests: test_finitediff.py test_fornberg
	@./test_fornberg
	@env PYTHONPATH=${CURDIR}/../..:${PYTHONPATH} python test_finitediff.py

test_fornberg: ../fornberg.f90 test_fornberg.f90
	$(FC) $(FFLAGS) -o $@ $^
