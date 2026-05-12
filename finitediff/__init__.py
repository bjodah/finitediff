# -*- coding: utf-8 -*-
"""
Finite difference weights for any derivative order on arbitrarily spaced grids.
"""

from pathlib import Path
from ._release import __version__

from ._finitediff_c import (
    derivatives_at_point_by_finite_diff,
    interpolate_by_finite_diff,
    get_weights,
)

__all__ = [
    "derivatives_at_point_by_finite_diff",
    "interpolate_by_finite_diff",
    "get_weights",
]


def get_include():
    inc_dir = Path(__file__).parent.resolve() / "include"
    if not inc_dir.is_dir():
        raise ValueError("Could not find include directory (as a dir): %s" % inc_dir)
    return str(inc_dir)
