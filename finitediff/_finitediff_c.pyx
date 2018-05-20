# -*- coding: utf-8 -*-
# distutils: sources = ['finitediff/external/newton_interval/src/newton_interval.c', 'src/finitediff_c.c']

cimport numpy as cnp
import numpy as np

from newton_interval cimport get_interval, get_interval_from_guess
from finitediff_c cimport apply_fd, calculate_weights


def get_weights(grid, double xtgt, int n=-1, int maxorder=0):
    """
    Generates finite differnece weights.

    Parameters
    ----------
    grid: array_like
        Grid points.
    xtgt: float
        Point at which estimates should be accurate.
    n: int, optional
        Number of points used in ``xarr``. default: -1 (means use length of xarr).
    maxorder: int, optional
        default: 0 (means interpolation)

    Returns
    -------
    array_like
         2 dimensional array with shape==(n, maxorder+1) with
         Fortran order (contiguous along columns)
         with weights for 0:th order in first column.
    """
    cdef cnp.ndarray[cnp.float64_t, ndim=1] xarr = numpy.ravel(grid)
    if n == -1:
        n = xarr.size
    cdef cnp.ndarray[cnp.float64_t, ndim=2, mode='fortran'] c = \
        np.empty((n, maxorder+1), order='F')
    calculate_weights(&c[0, 0], n, &xarr[0], n, maxorder, xtgt)
    return c


def derivatives_at_point_by_finite_diff(
        grid, ydata, double xtgt,
        int maxorder, yorder='C', squeeze=None):
    """ Esimates of derivatives up to specified order at a point.

    Estimates derivatives/function values of requested order
    at multiple points (xtgt) based on finite difference using
    provided ``grid`` and ``ydata``.

    Parameters
    ----------
    grid : array_like
        Grid points: values of the independent variable ("x-data").
    ydata : array_like
        Values of the dependent variable. May be two dimensional, in
        which case the weights of the grid is reused.
    xtgt : float
        The target value of the independent variable where the
        the finite difference scheme should be applied.
    maxorder : int, optional
        Maximum order of derivatives to estimate.
        The default is 0 (interpolation).
    yorder : char
        NumPy "order" of ydata.
    squeeze: bool
        Whether numpy.squeeze is applied to returned array. Default:
        ``True`` if ``ydata.ndim == 1``.


    Returns
    -------
    numpy.ndarray
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
    cdef cnp.ndarray[cnp.float64_t, ndim=1] xarr = np.ascontiguousarray(grid)
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yarr = np.ravel(ydata, order=yorder)
    if yarr.size % xarr.size:
        raise ValueError("Incompatible shapes: grid & ydata")
    cdef int nsets = yarr.size // xout.size
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yout = np.empty((maxorder+1)*nsets)
    if xdata.size != ydata.size:
        raise ValueError("xdata and ydata shapes incompatible")
    if xdata.size < maxorder+1:
        raise ValueError("xdata too short for requested derivative order")
    apply_fd(&yarr[0], xarr.size, nsets, maxorder, xarr.size, &xarr[0], &yarr[0], xarr.size, xtgt)
    result = yout.reshape((nsets, maxorder+1))
    if getattr(ydata, 'ndim', 1) == 1:
        return result.squeeze()
    else:
        return result


def interpolate_by_finite_diff(
        grid, ydata, xtgts, int maxorder=0, int ntail=2, int nhead=2, yorder='C'):
    """ Estimates derivatives of requested order at multiple points.

    Estimates derivatives/function values of requested order
    at multiple points (``xtgts``) based on finite difference using
    provided xdata and ydata.

    Parameters
    ----------
    grid : array_like
        Values of the independent variable ("x-data").
    ydata : array_like
        Values of the dependent variable.
    xtgts : array_like
        Values of the independent variable where the
        the finite difference scheme should be applied.
    maxorder : int, optional
        Up to what order derivatives are to be estimated.
        The default is 0 (interpolation).
    ntail : int, optional
        how many points in xdata before xout to inclued (default = 2).
    nhead : int, optional
        how many points in xdata after xout to include (default = 2).
    yorder : char
        NumPy "order" of ydata.
    squeeze: bool
        Whether numpy.squeeze is applied to returned array. Default:
        ``True`` if ``ydata.ndim == 1``.

    Returns
    -------
    array_like
        Estimates from applying the finite difference scheme

    Notes
    -----
    It is required that: ``order >= ntail + nhead``
    Algortithm assumes non-regularly spaced xdata. If
    xdata is regularly spaced this algortihm is not optimal
    from a performance perspective.

    References
    ----------
    The underlying algorithm is from:
    Generation of Finite Difference Formulas on Arbitrarily Spaced Grids,
    Bengt Fornberg, Mathematics of computation, 51, 184, 1988, 699-706
    """
    cdef int nin = ntail+nhead
    cdef int nout = xout.size
    cdef cnp.ndarray[cnp.float64_t, ndim=1] xarr = np.ascontiguousarray(grid)
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yarr = np.ravel(ydata, order=yorder)
    if yarr.size % xarr.size:
        raise ValueError("Incompatible shapes: grid & ydata")
    cdef int nsets = yarr.size // xout.size
    cdef cnp.ndarray[cnp.float64_t, ndim=3] yout = np.zeros(
        (nout, nsets, maxorder+1), order='C', dtype=np.float64)
    cdef int i, j

    if xdata.shape[0] < ntail+nhead:
        raise ValueError("ntail + nhead < xdata.shape[0]")
    if xdata.shape[0] != ydata.shape[0]:
        raise ValueError("xdata.shape[0] != ydata.shape[0]")
    if nhead+ntail < maxorder+1:
        raise ValueError("nhead+ntail < maxorder+1")

    for i in range(nout):
        j = max(0, get_interval_from_guess(
            &xdata[0], xdata.shape[0], xout[i], j))
        j = min(j, xdata.shape[0]-nin)
        apply_fd(
            &yout[i, 0, 0],
            maxorder+1,
            nsets,
            maxorder,
            nin,
            &xdata[j],
            &ydata[j],
            xarr.size,
            xout[i]
        )

    if getattr(ydata, 'ndim', 1) == 1:
        return yout.squeeze()
    else:
        return yout