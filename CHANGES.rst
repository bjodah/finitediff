v0.6.3
======
- update setup.py to re-run Cython when .pyx available
- update setup.py to new requirements in more recent versions of setuptools

v0.6.2
======
- Fix C macro bug

v0.6.1
======
- Support MSVC++ <10.0

v0.6.0
======
- New C API (prefixed function names with ``finitediff_``)
- New C function: finitediff_interpolate_by_finite_diff (optionally OpenMP parallelized)

v0.5.4
======
- Re-release for Zenodo

v0.5.3
======
- Update newton_interval with better check for Microsoft compiler

v0.5.2
======
- Update newton_interval to only require C89.

v0.5.1
======
- Include C++ header in Python distribution (backward compat.)

v0.5.0
======
- C89 implementation
- Efficient broadcasting from Python wrapper
- Windows support
- Documentation fixes

v0.4.0
======
- Removed ``*_optim`` functions. (since intermediate results were not used anyways)
- Python wrapper has a new utility subpackage: .grid
- Tweaked conda-recipe

v0.3.5
======
- Added (module) finitediff.grid

v0.3.4
======
- Fixes to source distribution

v0.3.3
======
- Improved setup.py

v0.3.2
======
- git-archive generated zip now pip-installable

v0.3.1
======
- Introduced ``finitediff::generate_weights_optim`` which is a pre-sorting
  wrapper around ``finitediff::generate_weights``  (tests indicate approx. 1 extra significant figure)
- Introduced ``finitediff::calculate_weights_optim`` which is a pre-sorting
  wrapper around ``finitediff::calculate_weights`` (tests indicate approx. 1 extra significant figure)

v0.3.0
======
- Refactored ``finitediff::generate_weights`` (part of official API from v0.3.0)
- Deprecated ``finitediff::populate_weights``
- Introduced ``finitediff::calculate_weights`` (to replace populate_weights)

v0.2.5
======
- Fixes to Python distribution (include C++ header)

v0.2.4
======
- C++11 function ``generate_weights`` provisionally provided

v0.2.3
======
- conda recipe related fixes

v0.2.2
======
- fixes to setup.py

v0.2.1
======
- More robust setup.py, source distributions better tested.

v0.2.0
======
- C++ backend used by default
- No external dependencies excpet numpy as cython by default
