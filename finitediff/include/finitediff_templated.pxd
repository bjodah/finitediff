# -*- coding: utf-8; mode: cython -*-

cdef extern from "finitediff_templated.hpp" namespace "finitediff":
     cdef void populate_weights[T](const T, const T * const,
                                   const int, const int,
                                   T * const)
     cdef void apply_fd[T](const int, const int,
                           const T * const, const T * const,
                           const T, T * const)
