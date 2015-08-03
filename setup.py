#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys

from distutils.core import setup

USE_FORTRAN = os.environ.get('USE_FORTRAN', '0').lower() in ('1', 'true')

if USE_FORTRAN:
    interface = 'fort'
    sources = [
        './src/finitediff_fort.f90',
        './src/c_finitediff_fort.f90',
    ]
else:
    interface = 'templated'
    sources = []

sources += [
    './external/newton_interval/src/newton_interval.c',
    './finitediff/_finitediff_'+interface+'.pyx'
]

pkg_name = 'finitediff'
release_py_path = os.path.join(pkg_name, 'release.py')
exec(open(release_py_path).read())  # sets __version__

if '--help' in sys.argv[1:] or sys.argv[1] in ('--help-commands', 'egg_info', 'clean', '--version'):
    cmdclass_ = {}
    ext_modules_ = []
else:
    # e.g. egg_info must not import from dependencies (pycompilation)
    import numpy
    from pycodeexport import pce_build_ext, PCEExtension
    from pycompilation.util import ArbitraryDepthGlob

    cmdclass_ = {'build_ext': pc_build_ext}
    ext_modules_ = [
        PCExtension(
            'finitediff._finitediff_'+interface,
            sources=sources,
            pycompilation_compile_kwargs={
                'per_file_kwargs': {
                    ArbitraryDepthGlob(b'*.c'): {'std': 'c99'}
                }
            },
            include_dirs=[
                './include',
                './external/newton_interval/include',
                numpy.get_include()
            ],
            language=None if USE_FORTRAN else 'c++',
            logger=True
        )
    ]

setup(
    name=pkg_name,
    version=__version__,
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    description='Finite difference weights for any derivative order on arbitrarily spaced grids.',
    license="BSD",
    url='https://github.com/bjodah/'+pkg_name,
    download_url='https://github.com/bjodah/'+pkg_name+'/archive/v'+__version__+'.tar.gz',
    packages=[pkg_name],
    cmdclass=cmdclass_,
    ext_modules=ext_modules_
)
