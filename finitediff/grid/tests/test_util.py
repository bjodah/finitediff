from .. import adapted_grid
from ..util import locate_discontinuity, pool_discontinuity_approx
from ._common import g3


def test_locate_discontinuity__pool_discontinuity_approx():
    for snr in [False, True]:
        loc_res = locate_discontinuity(
            *adapted_grid(0, 10, g3, grid_additions=(16,) * 8, snr=snr), consider=5
        )
        avg, s = pool_discontinuity_approx(loc_res)
        if snr:
            assert abs(avg - 5) < 0.03
        else:
            assert abs(avg - 5) > 1  # this is a negative test, i.e. not strict!
