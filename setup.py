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
srcs = ['fornberg.f90', 'newton_interval/src/newton_interval.c', '_finitediff.pyx']
inc_dirs=[os.path.join(abs_package_dir, 'newton_interval/include')]


class my_build_ext(build_ext.build_ext):
    def run(self):
        if not self.dry_run: # honor the --dry-run flag
            build_dir = os.path.join(
                os.path.abspath(os.path.dirname(__file__)),
                'build')
            if not os.path.exists(build_dir): os.mkdir(build_dir)
            abs_build_dir = os.path.abspath(build_dir)
            for f in srcs:
                copy(f, abs_build_dir, cwd=package_dir)
            src_objs = compile_sources(
                map(os.path.basename, srcs),
                options=['pic', 'warn', 'fast'],
                cwd=abs_build_dir, inc_dirs=inc_dirs)
            abs_so_path = link_py_so(src_objs, cwd=build_dir, fort=True)
            if self.inplace:
                copy(abs_so_path, abs_package_dir)
            else:
                print "only --inplace supported for now"
                raise NotImplementedError

setup(
    name='finitediff',
    version='0.1',
    description='Python extension for optimized inter-/extrapolation of data series for up to N-th order derivative.',
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    url='https://github.com/bjodah/finitediff',
    packages=['finitediff'],
    cmdclass = {'build_ext': my_build_ext},
)
