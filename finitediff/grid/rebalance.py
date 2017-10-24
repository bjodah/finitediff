# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import math
import numpy as np
from scipy.interpolate import interp1d
from ..util import interpolate_ahead


def rebalanced_grid(grid, err, base=0.25, num=None, resolution_factor=10, smooth_fact=1.0):
    if num is None:
        num = grid.size

    dx = grid.diff()
    area_err = 0.5*(err[1:] + err[:-1])*dx  # trapezoidal rule
    dx2 = np.zeros_like(grid)
    dx2[0], dx2[-1] = dx[0], dx[-1]
    dx2[1:-1] = 0.5*(dx[1:] + dx[:-1])

    def smooth_err(x):
        tot = 0
        for i, (gx, e) in enumerate(zip(grid, err)):
            fwhm = dx2[i]*smooth_fact
            tot += e*np.exp(-(x-gx)**2/(2*(fwhm/2.35482)**2))

    finegrid = np.zeros(grid.size * resolution_factor)
    for i in range(grid.size):
        finegrid[i*resolution_factor:(i+1)*resolution_factor] = np.linspace(
            grid[i], grid[i+1], resolution_factor+1)[:-1]
    smoothed = smooth_err(finegrid) + base*area_err/(grid[-1] - grid[0])
    interr = smoothed @ finegrid.diff()
    cb = interp1d(interr*num/interr[-1], finegrid)
    return cb(np.linspace(grid[0], grid[-1], num))
