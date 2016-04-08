# -*- coding: utf-8 -*-
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
