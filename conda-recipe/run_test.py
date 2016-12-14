import numpy as np
from finitediff import get_weights

c = get_weights(np.array([5., 6., 7.]), 5, maxorder=1)
assert np.allclose(c[:, 1], np.array([-3./2, 2, -1./2]))
