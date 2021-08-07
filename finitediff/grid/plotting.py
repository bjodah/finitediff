# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np
from .make import adapted_grid


def plot_convergence(key, values, cb, metric=None, xstart=0, xstop=2, **kwargs):
    import matplotlib.pyplot as plt

    if key in kwargs:
        raise ValueError("Cannot have key=%s when given in kwargs" % key)
    fig, axes = plt.subplots(
        1, len(values), figsize=(16, 5), sharey=True, gridspec_kw={"wspace": 0}
    )
    scores, grid_sizes = [], []
    if key is None and len(values) != 1:
        raise ValueError("Not considering key")
    for val, ax in zip(values, np.atleast_1d(axes)):
        if key is not None:
            kwargs[key] = val
        grid, results = adapted_grid(xstart, xstop, cb, metric=metric, **kwargs)
        y = results if metric is None else np.array([metric(r) for r in results])
        ax.vlines(
            grid,
            0,
            1,
            transform=ax.get_xaxis_transform(),
            linestyle="--",
            color="k",
            alpha=0.3,
            linewidth=0.5,
        )
        ax.plot(grid, y)
        between_x = grid[:-1] + np.diff(grid) / 2
        between_y = y[:-1] + np.diff(y) / 2
        rbx = cb(between_x)
        ybx = rbx if metric is None else np.array([metric(r) for r in rbx])
        scores.append(np.sum(np.abs(between_y - ybx)))
        grid_sizes.append(grid.size)
    return np.array(scores), grid_sizes
