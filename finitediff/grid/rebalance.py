# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import math
import numpy as np
from scipy.interpolate import interp1d

def _avgdiff(x):
    dx = np.diff(x)
    dx2 = np.zeros_like(x)
    dx2[0], dx2[-1] = dx[0], dx[-1]
    dx2[1:-1] = 0.5*(dx[1:] + dx[:-1])
    return dx2


def rebalanced_grid(grid, err, base=0.25, num=None, resolution_factor=10, smooth_fact=1.0):
    if num is None:
        num = grid.size

    dx = np.diff(grid)
    area_err = 0.5*(err[1:] + err[:-1]) @ dx  # trapezoidal rule
    dx2 = _avgdiff(grid)

    def smooth_err(x):
        tot = 0
        for i, (gx, e) in enumerate(zip(grid, err)):
            fwhm = dx2[i]*smooth_fact
            tot += e*np.exp(-(x-gx)**2/(2*(fwhm/2.35482)**2))
        return tot

    finegrid = np.zeros((grid.size-1) * resolution_factor + 1)
    for i in range(grid.size-1):
        finegrid[i*resolution_factor:(i+1)*resolution_factor] = np.linspace(
            grid[i], grid[i+1], resolution_factor+1)[:-1]
    finegrid[-resolution_factor-1:] = np.linspace(grid[-2], grid[-1], resolution_factor + 1)
    smoothed = smooth_err(finegrid) + base*area_err/(grid[-1] - grid[0])
    assert np.all(smoothed > 0)
    assert np.all(_avgdiff(finegrid) > 0)
    interr = np.cumsum(smoothed * _avgdiff(finegrid))
    cb = interp1d(interr, finegrid)
    return cb(np.linspace(interr[0], interr[-1], num))


def grid_pruning_mask(grid, err, ndrop=None, protect_sparse=None, pow_err=2, pow_dx=2):
    """ Returns a mask for grid pruning.

    Parameters
    ----------
    grid : array
    err : array
    ndrop : int
        If not provided taken as 25% of grid size (rounded upward).
    protect_sparse : int
        If not provided taken as 25% of grid size (rounded upward).
    pow_err : number
        Exponent of error in weighting.
    pow_dx : number
        Exponent of grid spacing in weighting.
    """
    if ndrop is None:
        ndrop = math.ceil(grid.size * 0.25)
    if protect_sparse is None:
        protect_sparse = math.ceil(grid.size * 0.25)
    dx = _avgdiff(grid)
    protected = np.argsort(dx)[-protect_sparse:]
    score = err**pow_err * dx**pow_dx
    importance = np.argsort(score)
    drop = []
    for considered in importance:
        if considered in protected:
            continue
        if considered - 1 in drop or considered + 1 in drop:
            continue
        drop.append(considered)
        if len(drop) == ndrop:
            break
    return ~np.in1d(np.arange(grid.size), drop)
