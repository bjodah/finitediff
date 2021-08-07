# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np

from ..util import interpolate_ahead


def refine_grid(
    grid,
    cb,
    grid_additions=(50, 50),
    ntrail=2,
    blurs=((), ()),
    metric=None,
    atol=None,
    rtol=None,
    extremum_refinement=None,
    snr=False,
):
    """Refines an existing grid by adding points to it.

    Parameters
    ----------
    grid : array
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

    Returns
    -------
    (grid, errors)

    """
    for na in grid_additions:
        if (na % 2) != 0:
            raise ValueError("Need even number of grid points for each addition")
    if extremum_refinement == "max":
        extremum_refinement = (np.argmax, 1, lambda y, i: True)
    elif extremum_refinement == "min":
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
            nextgrid[ptr : ptr + nloc + 1] = np.linspace(
                grd[gi], grd[gi + 1], 2 + nloc
            )[1:]
            nextresults[ptr + nloc] = res[gi + 1]
            nexty[ptr + nloc] = ys[gi + 1]
            if nloc > 0:
                yslices.append(slice(ptr, ptr + nloc))
            ptr += nloc + 1
        newresults = cb(np.concatenate([nextgrid[yslc] for yslc in yslices]))
        newy = (
            newresults if metric is None else np.array([metric(r) for r in newresults])
        )
        ystart, ystop = 0, 0
        for yslc in yslices:
            ystop += yslc.stop - yslc.start
            nextresults[yslc] = newresults[ystart:ystop]
            nexty[yslc] = newy[ystart:ystop]
            ystart = ystop

        return nextgrid, nextresults, nexty

    results = cb(grid)
    y = np.array(
        results if metric is None else [metric(r) for r in results], dtype=np.float64
    )

    for na in grid_additions:
        if extremum_refinement:
            extremum_cb, extremum_n, predicate_cb = extremum_refinement
            argext = extremum_cb(y)
            if predicate_cb(y, argext):
                additions = np.zeros(grid.size - 1, dtype=int)
                if argext > 0:  # left of
                    additions[argext - 1] = extremum_n
                elif argext < grid.size - 1:  # right of
                    additions[argext] = extremum_n
                grid, results, y = add_to(additions, grid, results, y)

        additions = np.zeros(grid.size - 1, dtype=int)
        done = True if atol is not None or rtol is not None else False
        slcs, errs = [], []
        for direction in ("fw", "bw"):
            est, slc = interpolate_ahead(grid, y, ntrail, direction)
            err = np.abs(y[slc] - est)
            if atol is not None:
                done = done and np.all(err < atol)
            if rtol is not None:
                done = done and np.all(err / y[slc] < rtol)
            slcs.append(slc)
            errs.append(err)

        if snr:
            all_errs = np.array(
                [[0.0] * ntrail + errs[0].tolist(), errs[1].tolist() + [0.0] * ntrail]
            )
            min__max = np.amin(all_errs, axis=0) / np.amax(all_errs, axis=0)
            dgrid = np.diff(grid)
            delta = np.empty_like(grid)
            delta[0] = dgrid[0] ** -2
            delta[-1] = dgrid[-1] ** -2
            delta[1:-1] = 1 / (dgrid[:-1] * dgrid[1:])
            lndelta = np.log(delta)
            normlndelta = lndelta - np.max(lndelta)

            for i in range(2):
                errs[i] *= (1.0 + 1e-8) - min__max[slcs[i]]
                errs[i] *= np.exp(normlndelta[slcs[i]])

        for direction, blur, slc, err in zip(("fw", "bw"), blurs, slcs, errs):
            for ib, b in enumerate(blur, 1):
                blur_slices = (slice(ib, None), slice(None, -ib))
                err[blur_slices[direction == "bw"]] += (
                    b * err[blur_slices[direction == "fw"]]
                )
            rerr = np.array(np.round(err * na / 2 / np.sum(err)), dtype=int)
            delta = np.sum(rerr) - na // 2
            if delta == 0:
                pass
            else:
                sorted_indices = np.argsort(rerr)
                for i in sorted_indices[-abs(delta) :]:
                    rerr[i] += 1 if delta < 0 else -1
                if np.sum(rerr) - na // 2:
                    raise ValueError("Balancing failed.")
            additions[
                slice(ntrail - 1, None)
                if direction == "fw"
                else slice(None, 1 - ntrail)
            ] += rerr
        grid, results, y = add_to(additions, grid, results, y)
        if done:
            break
    return grid, results
