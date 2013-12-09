# -*- coding: utf-8 -*-

# For wrapping fornberg.f90

cimport numpy as cnp
import numpy as np

from newton_interval cimport get_interval, get_interval_from_guess


cdef extern void apply_fd(int * nin, int * maxorder, double * xdata, double * ydata, double * xtgt, double * out)

cdef extern void populate_weights(double * z, double * x, int * nd,
                                  int * m, double * c)

def get_weights(double [::1] xarr, double xtgt, int n, int maxorder=0):
    cdef cnp.ndarray[cnp.float64_t, ndim=2, mode='fortran'] c = \
        np.zeros((n, maxorder+1), order='F')
    cdef int nm1 = n-1 # n minus 1
    populate_weights(&xtgt, &xarr[0], &nm1, &maxorder, &c[0,0])
    return c


cdef bint is_equidistant(double [:] x, double abstol=1e-9,
                         double reltol=1e-9):
    cdef int i
    cdef double dx
    cdef double rdx = x[1]-x[0] # ref dx
    if rdx == 0.0:
        return False
    for i in range(2,x.shape[0]):
        dx = x[i]-x[i-1]
        if abs(rdx-dx) > abstol or abs(dx/rdx-1.0) > reltol:
            return False
    return True


def derivatives_at_point_by_finite_diff(
        double [::1] xdata, double [::1] ydata, double xout,
        int maxorder):
    """
    >>> derivatives_at_point_by_finite_diff(np.array([.0, .5, 1.]),
            np.array([.0, .25, 1.]), .5, 2) # y=x**2
    array([.25, 1.0, 2.0]) # (x**2, 2x, 2)
    """
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yout = np.zeros(maxorder+1)
    assert xdata.size == ydata.size
    assert xdata.size >= maxorder+1
    cdef int nin = xdata.size
    apply_fd(&nin, &maxorder, &xdata[0], &ydata[0], &xout, &yout[0])
    return yout


def interpolate_by_finite_diff(
        double [::1] xdata, double [::1] ydata, double [::1] xout,
        int maxorder=0, int ntail=2, int nhead=2):
    """
    Interpolates function value (or its derivative - `order`)
    at xout based on finite difference using provided xdata and
    ydata. Algortithm assumes non-regularly spaced xdata. If
    xdata is regularly spaced this algortihm is not the optimal
    to use with respect to performance.

    The underlying algorithm is from:
    Generation of Finite Difference Formulas on Arbitrarily
        Spaced Grids, Bengt Fornberg
    Mathematics of compuation, 51, 184, 1988, 699-706
    """
    cdef int nin = ntail+nhead
    cdef int nout = xout.size
    cdef cnp.ndarray[cnp.float64_t, ndim=1] out = np.zeros(maxorder+1)
    cdef cnp.ndarray[cnp.float64_t, ndim=2] yout = \
        np.zeros((nout, maxorder+1), order='C')
    cdef int i,j # i,j are counters
    cdef double xtgt

    assert xdata.shape[0] >= ntail+nhead
    assert xdata.shape[0] == ydata.shape[0]
    assert nhead+ntail >= maxorder+1

    for i in range(nout):
        xtgt=xout[i]
        j = max(0, get_interval_from_guess(
            &xdata[0],xdata.shape[0], xtgt, j))
        j = min(j, xdata.shape[0]-nin)
        apply_fd(&nin,
                 &maxorder,
                 &xdata[j],
                 &ydata[j],
                 &xtgt,
                 &out[0])
        yout[i,:] = out
    return yout
