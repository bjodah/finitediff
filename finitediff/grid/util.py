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
