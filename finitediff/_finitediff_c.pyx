# -*- coding: utf-8 -*-
# distutils: sources = ['finitediff/external/newton_interval/src/newton_interval.c', 'src/finitediff_c.c']

cimport numpy as cnp
import numpy as np

from newton_interval cimport get_interval, get_interval_from_guess
from finitediff_c cimport apply_fd, calculate_weights


def get_weights(double [::1] xarr, double xtgt, int n=-1, int maxorder=0):
    """
    Generates finite differnece weights.

    Parameters
    ----------
    xarr: array_like
    xtgt: float
    n: int, optional
        default: -1 (means use length of xarr)
    maxorder: int, optional
        default: 0 (means interpolation)

    Returns
    -------
    array_like
         2 dimensional array with shape==(n, maxorder) with
         Fortran order (contiguous along columns)
         with weights for 0:th order in first column.
    """
    if n == -1:
        n = xarr.size
    cdef cnp.ndarray[cnp.float64_t, ndim=2, mode='fortran'] c = \
        np.empty((n, maxorder+1), order='F')
    calculate_weights(&c[0, 0], n, &xarr[0], n, maxorder, xtgt)
    return c


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
            np.array([.0, .25, 1.]), .5, 2)  # y=x**2
    array([.25, 1.0, 2.0])  # (x**2, 2x, 2)

    References
    ----------
    The underlying algorithm is from:
    Generation of Finite Difference Formulas on Arbitrarily Spaced Grids,
    Bengt Fornberg, Mathematics of compuation, 51, 184, 1988, 699-706
    """
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yout = np.empty(order+1)
    if xdata.size != ydata.size:
        raise ValueError("xdata and ydata shapes incompatible")
    if xdata.size < order+1:
        raise ValueError("xdata too short for requested derivative order")
    apply_fd(&yout[0], yout.size, 1, order, xdata.size, &xdata[0], &ydata[0], ydata.size, xout)
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
    Bengt Fornberg, Mathematics of computation, 51, 184, 1988, 699-706
    """
    cdef int nin = ntail+nhead
    cdef int nout = xout.size
    cdef cnp.ndarray[cnp.float64_t, ndim=1] out = np.empty(
        order+1, dtype=np.float64)
    cdef cnp.ndarray[cnp.float64_t, ndim=2] yout =np.zeros(
        (nout, order+1), order='C', dtype=np.float64)
    cdef int i, j # i,j are counters

    if xdata.shape[0] < ntail+nhead:
        raise ValueError("ntail + nhead < xdata.shape[0]")
    if xdata.shape[0] != ydata.shape[0]:
        raise ValueError("xdata.shape[0] != ydata.shape[0]")
    if nhead+ntail < order+1:
        raise ValueError("nhead+ntail < order+1")

    for i in range(nout):
        j = max(0, get_interval_from_guess(
            &xdata[0], xdata.shape[0], xout[i], j))
        j = min(j, xdata.shape[0]-nin)
        apply_fd(
            &out[0],
            out.shape[0],
            1,
            order,
            nin,
            &xdata[j],
            &ydata[j],
            order + 1,
            xout[i]
        )
        yout[i, :] = out
    return yout
