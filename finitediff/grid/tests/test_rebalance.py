# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np
import pytest

from ..rebalance import pre_pruning_mask


def test_pre_pruning_mask():
    assert np.all(pre_pruning_mask(np.array([0.0, 1e00, 2e00, 3, 4])) == [True] * 5)
    assert np.all(
        pre_pruning_mask(np.array([0.0, 1e-14, 2e00, 3, 4]), atol=1e-12)
        == [True, False, True, True, True]
    )
    assert np.all(
        pre_pruning_mask(np.array([0.0, 1e-14, 2e-14, 3, 4]), atol=1e-12)
        == [True, False, False, True, True]
    )
    assert np.all(
        pre_pruning_mask(np.array([0.0, 1e00, 2e00, 4, 4 + 2e-12]))
        == [True, True, True, False, True]
    )
    assert np.all(
        pre_pruning_mask(np.array([0.0, 1e00, 4, 4 + 1e-12, 4 + 2e-12]))
        == [True, True, False, False, True]
    )
    assert np.all(
        pre_pruning_mask(np.array([0.0, 1e-14, 2e00, 4, 4 + 2e-12]), atol=1e-12)
        == [True, False, True, False, True]
    )

    with pytest.raises(ValueError):
        assert pre_pruning_mask(np.array([1.0, 1 + 1e-13, 1 + 2e-13]))
