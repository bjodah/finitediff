# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import numpy as np
from .util import interpolate_ahead, avg_stddev


def adapted_grid(xstart, xstop, cb, grid_additions=(50, 50), ntrail=2, blurs=((), ()), metric=None,
                 atol=None, rtol=None):
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
    results = cb(grid)
    y = np.array(results if metric is None else [metric(r) for r in results], dtype=np.float64)
    for na in grid_additions[1:]:
        additions = np.zeros(grid.size - 1, dtype=int)
        done = True if atol is not None or rtol is not None else False
        for direction, blur in zip(('fw', 'bw'), blurs):
            est, slc = interpolate_ahead(grid, y, ntrail, direction)
            err = np.abs(y[slc] - est)
            if atol is not None:
                done = done and np.all(err < atol)
            if rtol is not None:
                done = done and np.all(err/y[slc] < rtol)
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
        nextresults = np.empty(grid.size + na, dtype=object)
        nextresults[0] = results[0]
        nexty = np.empty(grid.size + na)
        nexty[0] = y[0]
        nextgrid = np.empty(grid.size + na)
        nextgrid[0] = grid[0]
        ptr = 1
        yslices = []
        for gi, nloc in enumerate(additions):
            nextgrid[ptr:ptr+nloc+1] = np.linspace(grid[gi], grid[gi+1], 2+nloc)[1:]
            nextresults[ptr+nloc] = results[gi+1]
            nexty[ptr+nloc] = y[gi+1]
            if nloc > 0:
                yslices.append(slice(ptr, ptr+nloc))
            ptr += nloc + 1
        newresults = cb(np.concatenate([nextgrid[yslc] for yslc in yslices]))
        newy = newresults if metric is None else np.array([metric(r) for r in newresults])
        ystart, ystop = 0, 0
        for yslc in yslices:
            ystop += yslc.stop - yslc.start
            nextresults[yslc] = newresults[ystart:ystop]
            nexty[yslc] = newy[ystart:ystop]
            ystart = ystop

        grid = nextgrid
        results = nextresults
        y = nexty
        if done:
            break
    return grid, results


def locate_discontinuity(grid, y, consider, trnsfm=lambda x: x):
    tg = trnsfm(grid)
    dtg = np.diff(tg)
    dy = np.diff(y)
    dydtg = dy/dtg
    abs_dydtg = np.abs(dydtg)
    tgc = tg[:-1] + dtg/2
    imax = np.argsort(abs_dydtg)[-consider:][::-1]

    def w(m):
        if m == 0:
            return tgc[2]
        elif m == tgc.size - 1:
            return tgc[-1] - tgc[-3]
        else:
            return tgc[m+1] - tgc[m-1]

    return ([(tgc[m], w(m), dy[m]) for m in imax],
            [abs_dydtg[m]/abs_dydtg[imax[-1]] for m in imax[:-1]])


def pool_discontinuity_approx(loc_res, consistency_criterion=10):
    points, w2 = map(np.array, loc_res)
    w1 = np.abs(points[:, 2])
    avg1, stddev_avg1 = avg_stddev(points[:, 0], w1)
    avg2, stddev_avg2 = avg_stddev(points[:-1, 0], w2)
    avgerr1, stddev_avgerr1 = avg_stddev(points[:, 1], w1)
    avgerr2, stddev_avgerr2 = avg_stddev(points[:-1, 1], w2)
    if stddev_avg1/(avgerr1+stddev_avgerr1) > consistency_criterion:
        raise ValueError("Consistency criterion not met for avg1")
    if stddev_avg2/(avgerr1+stddev_avgerr2) > consistency_criterion:
        raise ValueError("Consistency criterion not met for avg1")
    return 0.5*(avg1 + avg2), stddev_avg1 + stddev_avg2 + 3*abs(avg1 - avg2)


def plot_convergence(key, values, cb, metric=None, **kwargs):
    import matplotlib.pyplot as plt

    if key in kwargs:
        raise ValueError("Cannot have key=%s when given in kwargs" % key)
    fig, axes = plt.subplots(1, len(values), figsize=(16, 5),
                             sharey=True, gridspec_kw={'wspace': 0})
    scores, grid_sizes = [], []
    for val, ax in zip(values, np.atleast_1d(axes)):
        kwargs[key] = val
        grid, results = adapted_grid(0, 2, cb, metric=metric, **kwargs)
        y = results if metric is None else np.array(
            [metric(r) for r in results])
        ax.vlines(grid, 0, 1, transform=ax.get_xaxis_transform(),
                  linestyle='--', color='k', alpha=.3, linewidth=.5)
        ax.plot(grid, y)
        between_x = grid[:-1] + np.diff(grid)/2
        between_y = y[:-1] + np.diff(y)/2
        rbx = cb(between_x)
        ybx = rbx if metric is None else np.array([metric(r) for r in rbx])
        scores.append(np.sum(np.abs(between_y - ybx)))
        grid_sizes.append(grid.size)
    return np.array(scores), grid_sizes
