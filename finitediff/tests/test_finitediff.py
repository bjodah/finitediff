# -*- coding: utf-8 -*-
#!/usr/bin/env python
from __future__ import print_function, division, absolute_import, unicode_literals

import numpy as np

from cInterpol import interpolate_by_finite_diff, derivatives_at_point_by_finite_diff

def test_derivatives_at_point_by_finite_diff():
    out = derivatives_at_point_by_finite_diff(
        np.array([.0, .5, 1.]),
        np.array([.0, .25, 1.]), .5, 2) # y=x**2
    assert np.allclose(np.array([.25, 1.0, 2.0]), out)

def test_interpolate_by_finite_diff():
    order = 0
    xarr = np.linspace(-1.5, 1.7, 53)
    yarr = np.exp(xarr)
    xtest = np.linspace(-1.4, 1.6, 57)
    y = interpolate_by_finite_diff(xarr, yarr, xtest,
                                   maxorder=4, ntail=5,
                                   nhead=5)
    yexact = np.exp(xtest)
    for ci in range(y.shape[1]):
        tol = 10**-(13-ci*2)
        assert np.allclose(yexact, y[:,ci],
                           rtol=tol, atol=tol)


if __name__ == '__main__':
    test_interpolate_by_finite_diff()
