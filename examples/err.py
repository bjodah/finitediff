#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np

from finitediff import derivatives_at_point_by_finite_diff


def demo_err():
    """
    This demo shows how the error in the estimate varies depending
    on how many data points are included in the interpolation
    (m parameter in this function).
    """
    max_order = 7
    n = 20
    lp = 0.25
    fmt1 = "{0: <5s}\t{1: <21s}\t{2: >21s}\t{3: >21s}\t{4: >21s}"
    fmt2 = "{0: <5d}\t{1:20.18f}\t{2: >21.18f}\t{3: >21.18f}\t{4: >21.18f}"
    x = np.cumsum(np.random.rand(n) * lp)
    x = np.concatenate((x[::-1] * -1, x))
    lst = []
    derivs = np.zeros(n)
    for order in range(max_order + 1):
        print("Order", order)
        for m in range(1 + order // 2, n + 1):
            sub_x = x[n - m : n + m]
            derivs[m - 1] = derivatives_at_point_by_finite_diff(
                sub_x, np.exp(sub_x), 0, order
            )[order]
        print(fmt1.format("m", "val", "diff", "analytical error", "diff/analytical"))
        for m in range(1, n):
            print(
                fmt2.format(
                    (m + 1) * 2,
                    derivs[m],
                    derivs[m] - derivs[m - 1],
                    derivs[m] - 1,
                    (derivs[m] - derivs[m - 1]) / (derivs[m] - 1),
                )
            )
        lst.append((derivs[-1], abs(derivs[-1] - derivs[-2])))
    print(np.array(lst))


if __name__ == "__main__":
    demo_err()
