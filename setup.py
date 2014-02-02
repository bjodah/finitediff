#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from distutils.core import setup

version_ = '0.1.8'
name_ = 'finitediff'

if '--help'in sys.argv[1:] or sys.argv[1] in ('--help-commands', 'egg_info', 'clean', '--version'):
    cmdclass_ = {}
    ext_modules_ = []
else:
    # e.g. egg_info must not import from dependencies (pycompilation)
    import numpy
    from pycompilation.dist import clever_build_ext
    from pycompilation.dist import CleverExtension
    from pycompilation.util import ArbitraryDepthGlob

    cmdclass_ = {'build_ext': clever_build_ext}
    ext_modules_ = [
        CleverExtension(
            "finitediff._finitediff",
            sources=[
                './finitediff/fornberg.f90',
                './finitediff/newton_interval/src/newton_interval.c',
                './finitediff/_finitediff.pyx'
            ],
            pycompilation_compile_kwargs={
                'per_file_kwargs': {
                    ArbitraryDepthGlob('*.c'): {'std': 'c99'}
                }
            },
            include_dirs=['./finitediff/newton_interval/include', numpy.get_include()])
    ]

setup(
    name=name_,
    version=version_,
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    description='Python extension for optimized inter-/extrapolation of data series for up to N-th order derivative.',
    license = "BSD",
    url='https://github.com/bjodah/'+name_,
    download_url='https://github.com/bjodah/'+name_+'/archive/v'+version_+'.tar.gz',
    packages=[name_],
    cmdclass = cmdclass_,
    ext_modules = ext_modules_
)
