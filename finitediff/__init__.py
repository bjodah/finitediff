# -*- coding: utf-8 -*-

from .release import __version__

try:
    from finitediff._finitediff_fort import (
        derivatives_at_point_by_finite_diff, interpolate_by_finite_diff,
        get_weights
    )
except ImportError:
    from finitediff._finitediff_templated import (
        derivatives_at_point_by_finite_diff, interpolate_by_finite_diff,
        get_weights
    )
