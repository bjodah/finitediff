# -*- coding: utf-8 -*-
"""
Finite difference weights for any derivative order on arbitrarily spaced grids.
"""
from __future__ import (absolute_import, division, print_function)

from ._release import __version__

try:
    from ._finitediff_templated import (
        derivatives_at_point_by_finite_diff, interpolate_by_finite_diff,
        get_weights
    )
except ImportError:
    from ._finitediff_fort import (
        derivatives_at_point_by_finite_diff, interpolate_by_finite_diff,
        get_weights
    )


def get_include():
    from pkg_resources import resource_filename, Requirement
    return resource_filename(Requirement.parse(__name__),
                             '%s/include' % __name__)
