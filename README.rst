==========
finitediff
==========
.. image:: https://travis-ci.org/bjodah/finitediff.png?branch=master
   :target: https://travis-ci.org/bjodah/finitediff

``finitediff`` containts two implementations ([Fortran
90](src/finitediff_fort.f90) and
[C++](include/finitediff_templated.hpp)) version of Begnt Fornberg's
formulae for generation of finite difference weights on aribtrarily
spaced one dimensional grids. The finite difference weights can be
used for optimized inter-/extrapolation data series for up to
arbitrary derivative order. Python_ bindings are provided.

.. _Python: https://www.python.org
.. _finitediff: https://github.com/bjodah/finitediff


Capabilities
============
finitediff currently provides callbacks for estimation of derivatives
or interpolation either at a single point or over an array (available 
from the Python bindings). 

The user may also manually generate the corresponding weights. (see
``populate_weights``) 


Installation
============
See dependencies and make sure that manual dependencies are satisfied.

Example using pip (modify to your own needs):

::

    $ pip install --user --upgrade -r https://raw.github.com/bjodah/finitediff/v0.1.10/requirements.txt
    $ pip install --user --upgrade https://github.com/bjodah/finitediff/archive/v0.1.10.tar.gz

Manual installation:


1. Clone repository ``$ git clone https://github.com/bjodah/finitediff.git``
2. Install dependencies ``$ cd finitediff; pip install --user --upgrade -r requirements.txt``
3. To install run ``$ python setup.py install --user`` or ``$ sudo python setup.py install``.

See distutils documentation_ for more options.

.. _documentation: http://docs.python.org/2/library/distutils.html

Note that the behaviour of ``setup(...)`` is modified slightly through the use of "PCExtension" from pycompilation_.


Tests
=====
To run the full test suite, you need to build the python
bindings. (see Dependencies)
::

    $ python setup.py build_ext --inplace
    $ py.test


Dependencies
============
You need either a C++ or a Fortran 90 compiler. On a debian based
linux system you can install it easily by typing: 
``$ sudo apt-get install gfortran g++`` 

Optional dependencies (for Python bindings):

- Python header files (``sudo apt-get install python-dev``)
- Python_ (tested with 2.7)
- NumPy_ 
- Cython_
- pycompilation_ (optional: enables use from python)
- pytest_ (``sudo apt-get install python-pytest``)
- matplotlib_ (``sudo apt-get install python-matplotlib``)

See [requirements.txt](requirements.txt) for detailed information of versions tested for.

For all dependencies the following command may be issued on a debian
based system:

::

    $ sudo apt-get install gfortran g++ python2.7 libpython2.7-dev
    python-numpy cython python-pip python-pytest python-matplotlib; sudo
    pip install --upgrade -r https://raw.github.com/bjodah/finitediff/v0.1.10/requirements.txt``


.. _NumPy: http://www.numpy.org/
.. _pycompilation: https://github.com/bjodah/pycompilation
.. _pytest: http://pytest.org/
.. _matplotlib: http://matplotlib.org/

Notes
=====
There is a git subtree under finitediff, update through:

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


Authors
=======
See file [AUTHORS](AUTHORS) in root
