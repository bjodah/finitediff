#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import, unicode_literals

import numpy as np

from finitediff import (
    interpolate_by_finite_diff,
    derivatives_at_point_by_finite_diff,
    get_weights,
)


def test_derivatives_at_point_by_finite_diff():
    out = derivatives_at_point_by_finite_diff(
        np.array([0.0, 0.5, 1.0]), np.array([0.0, 0.25, 1.0]), 0.5, 2
    )  # y=x**2
    assert np.allclose(np.array([0.25, 1.0, 2.0]), out)

    x = np.array([0.0, 0.5, 1.0, 1.5, 2.0])

    def f(x):
        return 3 + x - x ** 3

    pnt = 1.25
    out = derivatives_at_point_by_finite_diff(x, f(x), pnt, 2)

    def Df(x):
        return 1 - 3 * x ** 2

    def D2f(x):
        return -6 * x

    assert np.allclose([f(pnt), Df(pnt), D2f(pnt)], out)


def test_interpolate_by_finite_diff():
    xarr = np.linspace(-1.5, 1.7, 53)
    yarr = np.exp(xarr)
    xtest = np.linspace(-1.4, 1.6, 57)
    y = interpolate_by_finite_diff(
        xarr.tolist(), yarr.tolist(), xtest.tolist(), maxorder=4, ntail=5, nhead=5
    )
    if __name__ == "__main__":
        import matplotlib.pyplot as plt

        for ci in range(y.shape[1]):
            plt.plot(xtest, y[:, ci] - np.exp(xtest))
        plt.show()
    yexact = np.exp(xtest)
    for ci in range(y.shape[1]):
        tol = 10 ** -(13 - ci * 2)
        assert np.allclose(yexact, y[:, ci], rtol=tol, atol=tol)


def test_get_weights():
    c = get_weights(np.array([5.0, 6.0, 7.0]), 5, maxorder=1)
    assert np.allclose(c[:, 1], np.array([-3 / 2, 2, -1 / 2]))

    # Table 1, p. 702 in doi:10.1090/S0025-5718-1988-0935077-0
    # --------------------------------------------------------
    # x = [[0], [-1, 0, 1], ...]
    xl = [[j for j in range(-i, i + 1)] for i in range(0, 5)]

    # d holds all coefficients
    d = [
        get_weights(
            np.array(xl[i], dtype=np.float64),
            0,
            maxorder={0: 0, 1: 2, 2: 4, 3: 4, 4: 4}[i],
        )
        for i in range(5)
    ]

    def S(x):
        return 1.0 * x

    def test_d(d, i, o, r):
        print(d[i])
        assert np.allclose(d[i][:, o], np.array(r, dtype=np.float64))

    # Zeroeth derivative
    test_d(d, 0, 0, [1])

    # First derivative
    test_d(d, 1, 1, [-S(1) / 2, S(0), S(1) / 2])
    test_d(d, 2, 1, [S(1) / 12, -S(2) / 3, S(0), S(2) / 3, -S(1) / 12])
    test_d(
        d,
        3,
        1,
        [-S(1) / 60, S(3) / 20, -S(3) / 4, S(0), S(3) / 4, -S(3) / 20, S(1) / 60],
    )
    test_d(
        d,
        4,
        1,
        [
            S(1) / 280,
            -S(4) / 105,
            S(1) / 5,
            -S(4) / 5,
            S(0),
            S(4) / 5,
            -S(1) / 5,
            S(4) / 105,
            -S(1) / 280,
        ],
    )

    # Second derivative
    test_d(d, 1, 2, [S(1), -S(2), S(1)])
    test_d(d, 2, 2, [-S(1) / 12, S(4) / 3, -S(5) / 2, S(4) / 3, -S(1) / 12])
    test_d(
        d,
        3,
        2,
        [S(1) / 90, -S(3) / 20, S(3) / 2, -S(49) / 18, S(3) / 2, -S(3) / 20, S(1) / 90],
    )
    test_d(
        d,
        4,
        2,
        [
            -S(1) / 560,
            S(8) / 315,
            -S(1) / 5,
            S(8) / 5,
            -S(205) / 72,
            S(8) / 5,
            -S(1) / 5,
            S(8) / 315,
            -S(1) / 560,
        ],
    )

    # Third derivative
    test_d(d, 2, 3, [-S(1) / 2, S(1), S(0), -S(1), S(1) / 2])
    test_d(d, 3, 3, [S(1) / 8, -S(1), S(13) / 8, S(0), -S(13) / 8, S(1), -S(1) / 8])
    test_d(
        d,
        4,
        3,
        [
            -S(7) / 240,
            S(3) / 10,
            -S(169) / 120,
            S(61) / 30,
            S(0),
            -S(61) / 30,
            S(169) / 120,
            -S(3) / 10,
            S(7) / 240,
        ],
    )

    # Fourth derivative
    test_d(d, 2, 4, [S(1), -S(4), S(6), -S(4), S(1)])
    test_d(
        d, 3, 4, [-S(1) / 6, S(2), -S(13) / 2, S(28) / 3, -S(13) / 2, S(2), -S(1) / 6]
    )
    test_d(
        d,
        4,
        4,
        [
            S(7) / 240,
            -S(2) / 5,
            S(169) / 60,
            -S(122) / 15,
            S(91) / 8,
            -S(122) / 15,
            S(169) / 60,
            -S(2) / 5,
            S(7) / 240,
        ],
    )

    # Table 2, p. 703 in doi:10.1090/S0025-5718-1988-0935077-0
    # --------------------------------------------------------
    xl = [
        [j / S(2) for j in list(range(-i * 2 + 1, 0, 2)) + list(range(1, i * 2 + 1, 2))]
        for i in range(1, 5)
    ]

    # e holds all coefficients
    d = [
        get_weights(
            np.array(xl[i], dtype=np.float64), 0, maxorder={0: 1, 1: 2, 2: 4, 3: 4}[i]
        )
        for i in range(4)
    ]

    # Zeroth derivative
    test_d(d, 0, 0, [S(1) / 2, S(1) / 2])
    test_d(d, 1, 0, [-S(1) / 16, S(9) / 16, S(9) / 16, -S(1) / 16])
    test_d(
        d,
        2,
        0,
        [S(3) / 256, -S(25) / 256, S(75) / 128, S(75) / 128, -S(25) / 256, S(3) / 256],
    )
    test_d(
        d,
        3,
        0,
        [
            -S(5) / 2048,
            S(49) / 2048,
            -S(245) / 2048,
            S(1225) / 2048,
            S(1225) / 2048,
            -S(245) / 2048,
            S(49) / 2048,
            -S(5) / 2048,
        ],
    )

    # First derivative
    test_d(d, 0, 1, [-S(1), S(1)])
    test_d(d, 1, 1, [S(1) / 24, -S(9) / 8, S(9) / 8, -S(1) / 24])
    test_d(
        d,
        2,
        1,
        [-S(3) / 640, S(25) / 384, -S(75) / 64, S(75) / 64, -S(25) / 384, S(3) / 640],
    )
    test_d(
        d,
        3,
        1,
        [
            S(5) / 7168,
            -S(49) / 5120,
            S(245) / 3072,
            S(-1225) / 1024,
            S(1225) / 1024,
            -S(245) / 3072,
            S(49) / 5120,
            -S(5) / 7168,
        ],
    )

    # Reasonably the rest of the table is also correct... (testing of that
    # deemed excessive at the moment)


def test_interpolate_by_finite_diff__multiple_ydata__shape():
    x = np.array([0, 1, 2])
    y = np.array([[2, 3, 5], [3, 4, 7], [7, 8, 9], [3, 4, 6]])
    xout = np.linspace(0.5, 1.5, 5)
    r = interpolate_by_finite_diff(x, y, xout, maxorder=2)
    assert r.shape == (5, 4, 3)


def test_interpolate_by_finite_diff__multiple_ydata__data():
    xarr = np.linspace(-1.5, 1.7, 53)
    ny = 4
    xtest = np.linspace(-1.4, 1.6, 57)
    yarr = [i * np.exp(xarr) for i in range(1, ny + 1)]
    yexact = np.array([i * np.exp(xtest) for i in range(1, ny + 1)]).T
    maxorder = 4
    y = interpolate_by_finite_diff(
        xarr, yarr, xtest, maxorder=maxorder, ntail=5, nhead=5
    )
    assert y.shape == (xtest.size, ny, maxorder + 1)
    for ci in range(y.shape[2]):
        tol = 10 ** -(13 - ci * 2)
        assert np.allclose(yexact, y[..., ci], rtol=tol, atol=tol)


if __name__ == "__main__":
    test_interpolate_by_finite_diff()
    test_derivatives_at_point_by_finite_diff()
    test_get_weights()
