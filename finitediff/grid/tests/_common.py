# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import math
import numpy as np


def sigm(x, m, o, n):
    return m * (x - o) * ((m * (x - o)) ** n + 1) ** -(1.0 / n)


def g(x):
    s = sigm(x, 20, 1, 4)
    s2 = sigm(x, 5, 1, 4)
    g.nelem_call += x.size
    g.nfev += 1
    return 10 * (1 - s2 ** 2) * np.exp(20 * s) / np.exp(20 * x) + 0.1 * (x - 3) ** 2


g.nelem_call = 0
g.nfev = 0


def g2(x):
    return g(2 * x) + g(x)


g3 = np.vectorize(
    lambda t: (math.exp(-t) + (0.2 if round(t * 1234567) % 3 == 0 else 0))
    if t < 5
    else 0.5 * math.exp(5 - t)
)  # noisy function
