# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np
from .refine import refine_grid


def adapted_grid(xstart, xstop, cb, grid_additions=(50, 50), **kwargs):
    """ " Creates an adapted (1D) grid by subsequent subgrid insertions.

    Parameters
    ----------
    xstart : float
    xstop : float
    cb : callbable
        Function to be evaluated (note that noise is handled poorly).
    grid_additions : iterable of ints (even numbers)
        Sequence specifying how many gridpoints to add each time.
    \\*\\*kwargs : see :func:`refine_grid`.
    """
    grid = np.linspace(xstart, xstop, grid_additions[0])
    return refine_grid(grid, cb, grid_additions=grid_additions[1:], **kwargs)
