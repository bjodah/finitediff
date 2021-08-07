# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np
from . import interpolate_by_finite_diff


def interpolate_ahead(x, y, n, direction="fw"):
    if not np.all(np.diff(x) > 0):
        raise ValueError("x not strictly monotonic.")

    if direction == "both":
        y1, slc1 = interpolate_ahead(x, y, n, "fw")
        y2, slc2 = interpolate_ahead(x, y, n, "bw")
        y = np.zeros_like(x)
        y[slc1] = y1
        y[slc2] += y2
        y[slc1.start : slc2.stop] /= 2
        return y, slice(None, None)
    elif direction == "fw":
        forward = True
    elif direction == "bw":
        forward = False
    else:
        raise ValueError("Unknown direction: %s" % direction)

    rev = slice(None, None, 1) if forward else slice(None, None, -1)
    values = []
    for idx, (xv, yv) in enumerate(zip(x[rev][n:], y[rev][n:])):
        _x = np.ascontiguousarray(x[rev][idx : idx + n], dtype=np.float64)
        _y = np.ascontiguousarray(y[rev][idx : idx + n], dtype=np.float64)
        _v = np.array([xv])
        values.append(
            interpolate_by_finite_diff(_x, _y, _v, maxorder=0, ntail=n, nhead=0)
        )
    return np.array(values[rev]).squeeze(), slice(n, None) if forward else slice(
        None, -n
    )


def avg_stddev(arr, w):
    """Calculates the average and standard deviation.

    Parameters
    ----------
    arr : array_like
        Values.
    w : array_like
        Weights.

    Returns
    -------
    tuple of 2 floats (average & standard deviation)

    """
    avg, wsum = np.average(arr, weights=w, returned=True)
    res = arr - avg
    stddev = np.sqrt(np.sum(np.dot(w, np.square(res)) / (res.size - 1) / wsum))
    return avg, stddev
