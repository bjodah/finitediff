# Cython (www.cython.org) definition file
cdef extern from "newton_interval.h":
    int get_interval(const double * const arr, int N, const double t) nogil
    int get_interval_from_guess(const double * const arr, int N, double t, int i) nogil
    int check_nan(const double * const, int) nogil
    int check_strict_monotonicity(const double * const, int) nogil
