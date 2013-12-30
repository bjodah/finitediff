cdef extern int get_interval(const double * const arr, const size_t N, const double t)
cdef extern int get_interval_from_guess(const double * const arr, const size_t N, const double t, int i)
cdef extern int check_nan(const double * const, int)
cdef extern int check_strict_monotonicity(const double * const, int)
