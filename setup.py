#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

from distutils.core import setup
from distutils.command import build_ext

from pycompilation import pyx2obj, compile_sources, link_py_so
from pycompilation.util import copy

DEBUG=True
package_dir = 'finitediff'
abs_package_dir=os.path.join(os.path.abspath(os.path.dirname(__file__)), package_dir)
wrapper_src_path = '_finitediff.pyx'
srcs = ['fornberg.f90', 'newton_interval/src/newton_interval.c']
inc_dirs=[os.path.join(abs_package_dir, 'newton_interval/include')]


class my_build_ext(build_ext.build_ext):
    def run(self):
        if not self.dry_run: # honor the --dry-run flag
            build_dir = os.path.join(
                os.path.abspath(os.path.dirname(__file__)),
                'build')
            if not os.path.exists(build_dir): os.mkdir(build_dir)
            abs_build_dir = os.path.abspath(build_dir)
            pyx_obj = pyx2obj(wrapper_src_path, abs_build_dir,
                              metadir=abs_build_dir, cwd=package_dir,
                              inc_dirs=inc_dirs)
            src_objs = compile_sources(srcs, destdir=abs_build_dir,
                                       options=['pic', 'warn', 'fast'],
                                       metadir=abs_build_dir, cwd=package_dir,
                                       inc_dirs=inc_dirs)
            abs_so_path = link_py_so(src_objs+[pyx_obj], cwd=build_dir, fort=True)
            if self.inplace:
                copy(abs_so_path, abs_package_dir)
            else:
                print "only --inplace supported for now"
                raise NotImplementedError

setup(
    name='finitediff',
    version='0.0.1',
    description='Python extension for optimized inter-/extrapolation of data series for up to N-th order derivative.',
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    url='https://github.com/bjodah/finitediff',
    packages=['finitediff'],
    cmdclass = {'build_ext': my_build_ext},
)
