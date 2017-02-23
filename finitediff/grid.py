# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import numpy as np
from .util import interpolate_ahead


def adapted_grid(xstart, xstop, cb, grid_additions=(50, 50), ntrail=2, blurs=((), ())):
    """" Creates an adapted (1D) grid by subsequent subgrid insertions.

    Parameters
    ----------
    xstart : float
    xstop : float
    cb : callbable
    grid_additions : iterable of ints (even numbers)
    ntrail : int (>= 2)
    blurs : pair of iterables of ints (of same length)

    """
    for na in grid_additions:
        if (na % 2) != 0:
            raise ValueError('Need even number of grid points for each addition')

    grid = np.linspace(xstart, xstop, grid_additions[0])
    y = cb(grid)
    for na in grid_additions[1:]:
        additions = np.zeros(grid.size - 1, dtype=int)
        for direction, blur in zip(('fw', 'bw'), blurs):
            est, slc = interpolate_ahead(grid, y, ntrail, direction)
            err = np.abs(y[slc] - est)
            for ib, b in enumerate(blur, 1):
                blur_slices = (slice(ib, None), slice(None, -ib))
                err[blur_slices[direction == 'bw']] += b*err[blur_slices[direction == 'fw']]
            err *= na/2/np.sum(err)
            rerr = np.array(np.round(err), dtype=int)

            delta = np.sum(rerr) - na//2
            if delta == 0:
                pass
            else:
                sorted_indices = np.argsort(rerr)
                for i in sorted_indices[-abs(delta):]:
                    rerr[i] += 1 if delta < 0 else -1
                if (np.sum(rerr) - na//2):
                    raise ValueError('Balancing failed.')
            additions[slice(ntrail-1, None) if direction == 'fw' else slice(None, 1-ntrail)] += rerr
        nexty = np.empty(grid.size + na)
        nexty[0] = y[0]
        nextgrid = np.empty(grid.size + na)
        nextgrid[0] = grid[0]
        ptr = 1
        for gi, nloc in enumerate(additions):
            nextgrid[ptr:ptr+nloc+1] = np.linspace(grid[gi], grid[gi+1], 2+nloc)[1:]
            nexty[ptr+nloc] = y[gi+1]
            if nloc > 0:
                nexty[ptr:ptr+nloc] = cb(nextgrid[ptr:ptr+nloc])
            ptr += nloc + 1
        grid = nextgrid
        y = nexty
    return grid, y
