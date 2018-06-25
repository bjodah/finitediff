# -*- coding: utf-8; mode: cython -*-

cdef extern from "finitediff_c.h":
     cdef int finitediff_calculate_weights(double *, int, double *, int, int, double)
     cdef int finitediff_calc_and_apply_fd(double *, int, int, int, int, double *, double *, int, double)
     cdef int finitediff_interpolate_by_finite_diff(double * out, int, int, int, int, int, int, int, double *, int, double *, int, double *)
