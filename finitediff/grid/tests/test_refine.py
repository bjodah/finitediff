# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np
from ..refine import refine_grid
from ._common import g


def test_refine_grid():
    grid = np.linspace(0, 2, 8)
    rg, y = refine_grid(grid, g, (8,) * 3)
    assert rg.shape == (32,)
    assert y.shape == (32,)


def test_refine_grid__performance():
    r = []
    for ga in [(16,) * 2, (8,) * 4]:
        grid = np.linspace(0, 2, ga[0])
        grid, y = refine_grid(grid, g, ga[1:])
        bx = grid[:-1] + np.diff(grid) / 2
        by = y[:-1] + np.diff(y) / 2
        r.append(np.sum(np.abs(by - g(bx))))

    assert np.all(r < [0.272, 0.25, 0.15])
    assert np.all(np.diff(r) < 0)
