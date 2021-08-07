# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np
from ..make import adapted_grid
from ._common import g


def test_adapted_grid():
    ag, y = adapted_grid(0, 2, g, (8,) * 4)
    assert ag.shape == (32,)
    assert y.shape == (32,)


def test_adapted_grid__performance():
    r = []
    for ga in [(32,), (16,) * 2, (8,) * 4]:
        grid, y = adapted_grid(0, 2, g, ga)
        bx = grid[:-1] + np.diff(grid) / 2
        by = y[:-1] + np.diff(y) / 2
        r.append(np.sum(np.abs(by - g(bx))))

    assert np.all(r < [0.272, 0.25, 0.15])
    assert np.all(np.diff(r) < 0)
