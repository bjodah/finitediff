# -*- coding: utf-8 -*-

__version__ = '0.1.10-dev'

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
