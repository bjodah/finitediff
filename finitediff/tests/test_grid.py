# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import numpy as np
from ..grid import adapted_grid


def sigm(x, m, o, n):
    return m*(x-o)*((m*(x-o))**n + 1)**-(1./n)


def g(x):
    s = sigm(x, 20, 1, 4)
    s2 = sigm(x, 5, 1, 4)
    g.nelem_call += x.size
    g.nfev += 1
    return 10*(1-s2**2)*np.exp(20*s)/np.exp(20*x)+0.1*(x-3)**2

g.nelem_call = 0
g.nfev = 0

def g2(x):
    return g(2*x) + g(x)

def test_adapted_grid():
    ag, y = adapted_grid(0, 2, g, (8,)*4)
    assert ag.shape == (32,)
    assert y.shape == (32,)


def test_adapted_grid__performance():
    r = []
    for ga in [(32,), (16,)*2, (8,)*4]:
        grid, y = adapted_grid(0, 2, g, ga)
        bx = grid[:-1] + np.diff(grid)/2
        by = y[:-1] + np.diff(y)/2
        r.append(np.sum(np.abs(by - g(bx))))

    assert np.all(r < [.272, 0.25, .15])
    assert np.all(np.diff(r) < 0)


def test_adapted_grid__minimum_number_of_evals():
    g.nelem_call = 0
    g.nfev = 0
    adapted_grid(0, 2, g, (8,)*4)
    assert g.nelem_call == 8*4
    assert g.nfev == 4


def test_adapted_grid__metric():
    def gs(x):
        return [(a, b) for a, b in zip(g(x), g2(x))]
    grid, res = adapted_grid(0, 2, gs, (8,)*4, metric=lambda x: x[0] + x[1])
    assert grid.shape == (32,)
    assert np.array(res.tolist()).shape == (32, 2)
