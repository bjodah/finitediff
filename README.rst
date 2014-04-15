===========
finitediff
===========
.. image:: https://travis-ci.org/bjodah/finitediff.png?branch=master
   :target: https://travis-ci.org/bjodah/finitediff

"finitediff" is a Fortran 90 version of Begnt Fornberg's formulae for for optimized
inter-/extrapolation of data series for up to N-th order derivative. 
Native C/C++/Python bindings included (through ISO_C_BINDING and Cython_).

Feel free to enhance modify and make pull request at github to the finitediff_ repository.

.. _finitediff: https://github.com/bjodah/finitediff

__ finitediff_


Capabilities
============
finitediff currently provides callbacks for esimtation of derivatives or interpolation
either at a single point (see ``apply_fd`` in fornberg.f90) and over an array (in Python version).

The user may also manually generate the corresponding weights. (see ``populate_weights`` in fornberg.f90)


Installation
============
See dependencies and make sure that manual dependencies are satisfied.

Example using pip (modify to your own needs):
    1. ``pip install --user --upgrade -r https://raw.github.com/bjodah/finitediff/master/requirements.txt``
    2. ``pip install --user --upgrade https://github.com/bjodah/finitediff/archive/v0.1.9.tar.gz``

Manual installation:
    1. Clone repository ``git clone https://github.com/bjodah/finitediff.git``
    2. Install dependencies ``cd finitediff; pip install --user --upgrade -r requirements.txt``
    3. To install run ``python setup.py install --user`` or ``sudo python setup.py install``.

See distutils documentation_ for more options.

.. _documentation: http://docs.python.org/2/library/distutils.html

Note that the behaviour of ``setup(...)`` is modified slightly through the use of "CleverExtension" from pycompilation_.


Tests
=====
1. ``python setup.py build_ext --inplace``
2. ``py.test``

TODO
====
add more tests. Write documentation.


Dependencies
============
Manual dependencies:

- Python header files (sudo apt-get install python-dev)
- Fortran compiler (sudo apt-get install gfortran)


See requirements.txt for detailed information of versions tested for.

- Python_ (tested with 2.7)
- NumPy_ 
- Cython_
- pycompilation_ (optional: enables use from python)


Optional requirements:

- pytest_ (sudo apt-get install python-pytest)
- matplotlib_ (sudo apt-get install python-matplotlib)


.. _Python: http://www.python.org
.. _NumPy: http://www.numpy.org/
.. _Cython: http://www.cython.org/
.. _pycompilation: https://github.com/bjodah/pycompilation
.. _pytest: http://pytest.org/
.. _matplotlib: http://matplotlib.org/

Notes
=====
There is a git subtree under cInterpol, update through:

``git subtree pull --prefix finitediff/newton_interval newton_interval master --squash``

where the repo "newton_interval" is https://github.com/bjodah/newton_interval.git

First time you need to add it:
``git remote add -f newton_interval https://github.com/bjodah/newton_interval.git``

(Users of Ubuntu 12.04 who want to use git subtree, see http://stackoverflow.com/questions/17797328)

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


License
=======
Open Source. Released under the very permissive "simplified
(2-clause) BSD license". See LICENSE.txt for further details.


Author
======
Björn Dahlgren, contact (gmail adress): bjodah
