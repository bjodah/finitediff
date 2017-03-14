# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import numpy as np
from .util import interpolate_ahead, avg_stddev


def adapted_grid(xstart, xstop, cb, grid_additions=(50, 50), ntrail=2, blurs=((), ()), metric=None,
                 atol=None, rtol=None, extremum_refinement=None, snr=False):
    """" Creates an adapted (1D) grid by subsequent subgrid insertions.

    Parameters
    ----------
    xstart : float
    xstop : float
    cb : callbable
        Function to be evaluated (note that noise is handled poorly).
    grid_additions : iterable of ints (even numbers)
        Sequence specifying how many gridpoints to add each time.
    ntrail : int (>= 2)
        Number of points to include in the look-ahead extrapolation.
    blurs : pair of iterables of ints (of same length)
        Blur fractions of absolute residuals to neighbors.
    atol : float
        Absolute tolerance to be fulfilled by all absolute residuals for early exit.
    rtol : float
        Relative tolerance to be fulfilled by all absolute residuals for early exit.
    extremum_refinement : locator (callable), n (int), predicate (callable)
        Between each grid addition a callable for locating the extremum (e.g. np.argmax)
        can be evaluated. The integer specifies how many gridpoints that should be inserted
        on each side (one side if on boundary) of the extremum.
    snr : bool
        Use signal-to-noise ratio the lower the grid-addition-weight of potential noise.

    """
    for na in grid_additions:
        if (na % 2) != 0:
            raise ValueError('Need even number of grid points for each addition')
    if extremum_refinement == 'max':
        extremum_refinement = (np.argmax, 1, lambda y, i: True)
    elif extremum_refinement == 'min':
        extremum_refinement = (np.argmin, 1, lambda y, i: True)

    def add_to(adds, grd, res, ys):
        na = np.sum(adds)
        if na == 0:
            return grd, res, ys
        nextresults = np.empty(grd.size + na, dtype=object)
        nextresults[0] = res[0]
        nexty = np.empty(grd.size + na)
        nexty[0] = ys[0]
        nextgrid = np.empty(grd.size + na)
        nextgrid[0] = grd[0]
        ptr = 1
        yslices = []
        for gi, nloc in enumerate(adds):
            nextgrid[ptr:ptr+nloc+1] = np.linspace(grd[gi], grd[gi+1], 2+nloc)[1:]
            nextresults[ptr+nloc] = res[gi+1]
            nexty[ptr+nloc] = ys[gi+1]
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

        return nextgrid, nextresults, nexty

    grid = np.linspace(xstart, xstop, grid_additions[0])
    results = cb(grid)
    y = np.array(results if metric is None else [metric(r) for r in results], dtype=np.float64)

    for na in grid_additions[1:]:
        if extremum_refinement:
            extremum_cb, extremum_n, predicate_cb = extremum_refinement
            argext = extremum_cb(y)
            if predicate_cb(y, argext):
                additions = np.zeros(grid.size - 1, dtype=int)
                if argext > 0:  # left of
                    additions[argext-1] = extremum_n
                elif argext < grid.size - 1:  # right of
                    additions[argext] = extremum_n
                grid, results, y = add_to(additions, grid, results, y)

        additions = np.zeros(grid.size - 1, dtype=int)
        done = True if atol is not None or rtol is not None else False
        slcs, errs = [], []
        for direction in ('fw', 'bw'):
            est, slc = interpolate_ahead(grid, y, ntrail, direction)
            err = np.abs(y[slc] - est)
            if atol is not None:
                done = done and np.all(err < atol)
            if rtol is not None:
                done = done and np.all(err/y[slc] < rtol)
            slcs.append(slc)
            errs.append(err)

        if snr:
            all_errs = np.array([[.0]*ntrail + errs[0].tolist(), errs[1].tolist() + [.0]*ntrail])
            min__max = np.amin(all_errs, axis=0)/np.amax(all_errs, axis=0)
            dgrid = np.diff(grid)
            delta = np.empty_like(grid)
            delta[0] = dgrid[0]**-2
            delta[-1] = dgrid[-1]**-2
            delta[1:-1] = 1/(dgrid[:-1]*dgrid[1:])
            lndelta = np.log(delta)
            normlndelta = lndelta - np.max(lndelta)

            for i in range(2):
                errs[i] *= (1.0 + 1e-8) - min__max[slcs[i]]
                errs[i] *= np.exp(normlndelta[slcs[i]])

        for direction, blur, slc, err in zip(('fw', 'bw'), blurs, slcs, errs):
            for ib, b in enumerate(blur, 1):
                blur_slices = (slice(ib, None), slice(None, -ib))
                err[blur_slices[direction == 'bw']] += b*err[blur_slices[direction == 'fw']]
            rerr = np.array(np.round(err*na/2/np.sum(err)), dtype=int)
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
        grid, results, y = add_to(additions, grid, results, y)
        if done:
            break
    return grid, results


def locate_discontinuity(grid, y, consider, trnsfm=lambda x: x, ntrail=2):
    y = np.asarray(y, dtype=np.float64)
    dy = np.diff(y)
    tg = trnsfm(grid)
    dtg = np.diff(tg)
    err = np.zeros(y.size)
    for d in ('fw', 'bw'):
        est, slc = interpolate_ahead(tg, y, ntrail, d)
        start = (ntrail - 1) if d == 'fw' else 0
        stop = -(ntrail - 1) if d == 'bw' else None
        err[slc] += np.abs(y[slc] - est)/dtg[start:stop]*dy[start:stop]
    imax = np.argsort(err)[-consider:][::-1]
    return [(tg[m], err[m]) for m in imax]


def pool_discontinuity_approx(loc_res, consistency_criterion=10):
    points = np.array(loc_res)
    w1 = np.abs(points[:, 1])
    return avg_stddev(points[:, 0], w1)


def plot_convergence(key, values, cb, metric=None, xstart=0, xstop=2, **kwargs):
    import matplotlib.pyplot as plt

    if key in kwargs:
        raise ValueError("Cannot have key=%s when given in kwargs" % key)
    fig, axes = plt.subplots(1, len(values), figsize=(16, 5),
                             sharey=True, gridspec_kw={'wspace': 0})
    scores, grid_sizes = [], []
    if key is None and len(values) != 1:
        raise ValueError("Not considering key")
    for val, ax in zip(values, np.atleast_1d(axes)):
        if key is not None:
            kwargs[key] = val
        grid, results = adapted_grid(xstart, xstop, cb, metric=metric, **kwargs)
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
