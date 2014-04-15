# -*- coding: utf-8 -*-

# For wrapping fornberg.f90

cimport numpy as cnp
import numpy as np

from newton_interval cimport get_interval, get_interval_from_guess
from c_fornberg cimport fornberg_apply_fd, fornberg_populate_weights

def get_weights(double [::1] xarr, double xtgt, int n, int maxorder=0):
    """
    Generates finite differnece weights.

    Parameters
    ----------
    xarr: array_like
    xtgt: float
    n: int
    maxorder: int

    Returns
    -------
    array_like
         weights
    """
    cdef cnp.ndarray[cnp.float64_t, ndim=2, mode='fortran'] c = \
        np.zeros((n, maxorder+1), order='F')
    fornberg_populate_weights(xtgt, &xarr[0], n-1, maxorder, &c[0,0])
    return c


cdef bint is_equidistant(double [:] x, double abstol=1e-9,
                         double reltol=1e-9):
    """
    Parameters
    ----------
    x : array_like
         array to determine whether equidistantly spaced.
    abstol : float
         Absolute tolerance.
    reltol : float
         Relative tolerance.
    """
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
        int order):
    """
    Estimates derivatives/function values of requested order
    at multiple points (xout) based on finite difference using
    provided xdata and ydata.

    Parameters
    ==========
    xdata : array_like
        values of the independent variable

    ydata : array_like
        values of the dependent variable

    xout : float
        value of the independent variable where the
        the finite difference scheme should be applied.

    order : int, optional
        what order of derivatives to estimate.
        The default is 0 (interpolation)


    Returns
    -------
    float
        Estimate from applying the finite difference scheme.

    Examples
    --------
    >>> derivatives_at_point_by_finite_diff(np.array([.0, .5, 1.]),
            np.array([.0, .25, 1.]), .5, 2) # y=x**2
    array([.25, 1.0, 2.0]) # (x**2, 2x, 2)

    References
    ----------
    The underlying algorithm is from:
    Generation of Finite Difference Formulas on Arbitrarily Spaced Grids,
    Bengt Fornberg, Mathematics of compuation, 51, 184, 1988, 699-706
    """
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yout = np.zeros(order+1)
    assert xdata.size == ydata.size
    assert xdata.size >= order+1
    fornberg_apply_fd(xdata.size, order, &xdata[0], &ydata[0], xout, &yout[0])
    return yout


def interpolate_by_finite_diff(
        double [::1] xdata, double [::1] ydata, double [::1] xout,
        int order=0, int ntail=2, int nhead=2):
    """
    Estimates derivatives/function values of requested order
    at multiple points (xout) based on finite difference using
    provided xdata and ydata.

    Parameters
    ==========
    xdata : array_like
        values of the independent variable

    ydata : array_like
        values of the dependent variable

    xout : array_like
        values of the independent variable where the
        the finite difference scheme should be applied.

    order : int, optional
        what order of derivatives to estimate.
        The default is 0 (interpolation)

    ntail : int, optional
        how many points in xdata before xout to inclued (default = 2).

    nhead : int, optional
        how many points in xdata after xout to include (default = 2).


    Returns
    -------
    array_like
        Estimates from applying the finite difference scheme

    Notes
    -----
    It is required that: order >= ntail + nhead
    Algortithm assumes non-regularly spaced xdata. If
    xdata is regularly spaced this algortihm is not the optimal
    to use with respect to performance.

    References
    ----------
    The underlying algorithm is from:
    Generation of Finite Difference Formulas on Arbitrarily Spaced Grids,
    Bengt Fornberg, Mathematics of compuation, 51, 184, 1988, 699-706
    """
    cdef int nin = ntail+nhead
    cdef int nout = xout.size
    cdef cnp.ndarray[cnp.float64_t, ndim=1] out = np.zeros(order+1)
    cdef cnp.ndarray[cnp.float64_t, ndim=2] yout = \
        np.zeros((nout, order+1), order='C')
    cdef int i,j # i,j are counters

    assert xdata.shape[0] >= ntail+nhead
    assert xdata.shape[0] == ydata.shape[0]
    assert nhead+ntail >= order+1

    for i in range(nout):
        j = max(0, get_interval_from_guess(
            &xdata[0], xdata.shape[0], xout[i], j))
        j = min(j, xdata.shape[0]-nin)
        fornberg_apply_fd(
            nin,
            order,
            &xdata[j],
            &ydata[j],
            xout[i],
            &out[0])
        yout[i,:] = out
    return yout
