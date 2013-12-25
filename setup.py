#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from distutils.core import setup

version_ = '0.1.2'
name_ = 'finitediff'

if any([x in sys.argv for x in ('build', 'build_ext', 'install')]):
    # e.g. egg_info must not import from dependencies (pycompilation)
    from pycompilation.dist import clever_build_ext as build_ext
    from pycompilation.dist import CleverExtension as Extension
else:
    from distutils.command import build_ext
    from distutils.extension import Extension


cmdclass_ = {}
ext_modules_ = []

cmdclass_ = {'build_ext': build_ext}, 
ext_modules_ = [
    Extension(
        "finitediff._finitediff",
        sources=[
            'finitediff/fornberg.f90',
            'finitediff/newton_interval/src/newton_interval.c',
            'finitediff/_finitediff.pyx'
        ],
        include_dirs=['finitediff/newton_interval/include'])
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
