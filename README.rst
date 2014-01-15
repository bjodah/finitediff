===========
finitediff
===========

finitediff_ is a python extension for optimized inter-/extrapolation of
data series for up to N-th order derivative.

Feel free to enhance modify and make pull request at github to

.. _finitediff: https://github.com/bjodah/finitediff

__ finitediff_

References
==========
The algortihm is a Fortran 90 rewrite of:

http://dx.doi.org/10.1137/S0036144596322507

::

    @article{fornberg_classroom_1998,
      title={Classroom note: Calculation of weights in finite difference formulas},
      author={Fornberg, Bengt},
      journal={SIAM review},
      volume={40},
      number={3},
      pages={685--691},
      year={1998},
      publisher={SIAM}
      doi={10.1137/S0036144596322507}
    }
    

Which is based on an article of the same author:

http://dx.doi.org/10.1090/S0025-5718-1988-0935077-0

::

    @article{fornberg_generation_1988,
      title={Generation of finite difference formulas on arbitrarily spaced grids},
      author={Fornberg, Bengt},
      journal={Mathematics of computation},
      volume={51},
      number={184},
      pages={699--706},
      year={1988}
      doi={10.1090/S0025-5718-1988-0935077-0}
    }


Capabilities
============
finitediff currently provides callbacks for evaluation at single point and over an array


Installation
============
Example using pip (modify to your own needs):

    1. ``pip install --user --upgrade -r https://raw.github.com/bjodah/finitediff/master/requirements.txt``
    2. ``pip install --user --upgrade https://github.com/bjodah/finitediff/archive/v0.1.4.tar.gz``

Manual installation:
    1. Clone repository ``git clone https://github.com/bjodah/finitediff.git``
    2. Install dependencies ``cd finitediff; pip install --user --upgrade -r requirements.txt``
    3. To install run ``python setup.py install --user`` or ``sudo python setup.py install``.

See distutils' documentation_ for more options.
.. _documentation: http://docs.python.org/2/library/distutils.html

Note that the behaviour of ``setup(...)`` is modified slightly through the use of "CleverExtension" from pycompilation.


Tests
=====
TODO: improve tests.


Dependencies
============
See requirements.txt for detailed information of versions tested for.
* Python_ (tested with 2.7)
* NumPy_ 
* Cython_
* pycompilation_ (optional: enables use from python)

.. _Python: http://www.python.org
.. _NumPy: http://www.numpy.org/
.. _Cython: http://www.cython.org/
.. _pycompilation: https://github.com/bjodah/pycompilation


License
=======
Open Soucrce. Released under the very permissive "simplified
(2-clause) BSD license". See LICENCE.txt for further details.


Author
======
Björn Dahlgren, contact (gmail adress): bjodah
