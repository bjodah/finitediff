# -*- coding: utf-8 -*-
# distutils: sources = ['finitediff/external/newton_interval/src/newton_interval.c', 'src/finitediff_c.c']
# cython: language_level=3

cimport numpy as cnp
import numpy as np

from newton_interval cimport get_interval, get_interval_from_guess
from finitediff_c cimport finitediff_calc_and_apply_fd, finitediff_calculate_weights, finitediff_interpolate_by_finite_diff


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
    cdef cnp.ndarray[cnp.float64_t, ndim=1] xarr = np.ascontiguousarray(np.ravel(grid), dtype=np.float64)
    if n == -1:
        n = xarr.size
    cdef cnp.ndarray[cnp.float64_t, ndim=2, mode='fortran'] c = \
        np.empty((n, maxorder+1), order='F')
    finitediff_calculate_weights(&c[0, 0], n, &xarr[0], n, maxorder, xtgt)
    return c


def derivatives_at_point_by_finite_diff(
        grid, ydata, double xtgt,
        int maxorder, yorder='C', reshape=None):
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
    reshape: bool
        Whether to return a 2D array or not. Default:
        if ``ydata.ndim != 1``.


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
    ydata = np.asarray(ydata)
    cdef cnp.ndarray[cnp.float64_t, ndim=1] xarr = np.ascontiguousarray(grid, dtype=np.float64)
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yarr = np.ascontiguousarray(np.ravel(ydata, order=yorder), dtype=np.float64)
    if yarr.size % xarr.size:
        raise ValueError("Incompatible shapes: grid & ydata")
    cdef int nsets = yarr.size // xarr.size
    cdef cnp.ndarray[cnp.float64_t, ndim=1] yout = np.empty((maxorder+1)*nsets)
    if xarr.size < maxorder+1:
        raise ValueError("xdata too short for requested derivative order")
    finitediff_calc_and_apply_fd(&yout[0], xarr.size, nsets, maxorder, xarr.size, &xarr[0], &yarr[0], xarr.size, xtgt)
    if reshape is None:
        reshape = ydata.ndim != 1
    if reshape:
        return yout.reshape((nsets, maxorder+1))
    else:
        return yout

def interpolate_by_finite_diff(
        grid, ydata, xtgts, int maxorder=0, int ntail=2, int nhead=2, yorder='C', reshape=None):
    """ Estimates derivatives of requested order at multiple points.

    Estimates derivatives/function values of requested order
    at multiple points (``xtgts``) based on finite difference using
    provided ``grid`` and ``ydata``.

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
        how many points in ``grid`` before ``xtgts`` to inclued (default = 2).
    nhead : int, optional
        how many points in ``grid`` after ``xtgts`` to include (default = 2).
    yorder : char
        NumPy "order" of ydata.
    reshape: bool
        Whether to return a 3D array or not. Default:
        if ``ydata.ndim != 1``.

    Returns
    -------
    array_like
        Estimates from applying the finite difference scheme

    Examples
    --------
    >>> import numpy as np
    >>> from finitediff import interpolate_by_finite_diff as ifd
    >>> x = np.array([0, 1, 2])
    >>> y = np.array([[2, 3, 5], [3, 4, 7], [7, 8, 9], [3, 4, 6]])
    >>> xout = np.linspace(0.5, 1.5, 5)
    >>> r = ifd(x, y, xout, maxorder=2)
    >>> r.shape
    (5, 4, 3)

    Notes
    -----
    It is required that: ``order >= ntail + nhead``
    Algortithm assumes non-regularly spaced ``grid``. If
    ``grid`` is regularly spaced this algortihm is not optimal
    from a performance perspective.

    References
    ----------
    The underlying algorithm is from:
    Generation of Finite Difference Formulas on Arbitrarily Spaced Grids,
    Bengt Fornberg, Mathematics of computation, 51, 184, 1988, 699-706
    """
    ydata = np.asarray(ydata)
    xtgts = np.asarray(xtgts)
    cdef:
        int flag
        int nin = ntail+nhead
        int nout = xtgts.size
        cnp.ndarray[cnp.float64_t, ndim=1] xgrd = np.ascontiguousarray(grid, dtype=np.float64)
        cnp.ndarray[cnp.float64_t, ndim=1] tgts = np.ascontiguousarray(xtgts, dtype=np.float64)
        cnp.ndarray[cnp.float64_t, ndim=1] yarr = np.ascontiguousarray(np.ravel(ydata, order=yorder), dtype=np.float64)
        int nsets = yarr.size // xgrd.size
        cnp.ndarray[cnp.float64_t, ndim=1] yout = np.zeros(
            (nout*nsets*(maxorder+1)), order='C', dtype=np.float64)

    if yarr.size % xgrd.size:
        raise ValueError("Incompatible shapes: grid & ydata")

    flag = finitediff_interpolate_by_finite_diff(
        <double*>yout.data, nout, nsets, maxorder, nsets*(maxorder+1), maxorder+1,
        ntail, nhead, <double*>xgrd.data, xgrd.size, <double*>yarr.data, xgrd.size,
        <double*>tgts.data
    )
    if flag == 1:
        raise ValueError("Bad alloc")
    elif flag == 2:
        raise ValueError("grid is too small")
    if flag == 4:
        raise ValueError("too few points")

    if reshape is None:
        reshape = ydata.ndim != 1
    if reshape:
        return yout.reshape((nout, nsets, maxorder+1))
    else:
        return yout.reshape((nout, -1))
