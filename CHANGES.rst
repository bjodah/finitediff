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
