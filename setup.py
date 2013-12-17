#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.core import setup
from pycompilation.dist import clever_build_ext, CleverExtension


setup(
    name='finitediff',
    version='0.1.1',
    description='Python extension for optimized inter-/extrapolation of data series for up to N-th order derivative.',
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    url='https://github.com/bjodah/finitediff',
    packages=['finitediff'],
    cmdclass = {'build_ext': clever_build_ext},
    ext_modules = [
        CleverExtension(
            "finitediff._finitediff",
            sources=[
                'finitediff/fornberg.f90',
                'finitediff/newton_interval/src/newton_interval.c',
                'finitediff/_finitediff.pyx'
            ],
        include_dirs=['finitediff/newton_interval/include'])
    ]
)
