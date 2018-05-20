# -*- coding: utf-8; mode: cython -*-

cdef extern from "finitediff_c.h":
     cdef int calculate_weights(double *, int, double *, int, int, double)
     cdef int apply_fd(double *, int, int, int, int, double *, double *, int, double)
