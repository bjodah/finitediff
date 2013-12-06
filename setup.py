#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

from distutils.core import setup
from distutils.command import build_ext

from pycompilation import pyx2obj, src2obj, link_py_so
from pycompilation.util import copy

wrapper_src_path = '_finitediff.pyx'
srcs = ['fornberg.f90']

package_dir = 'finitediff'
DEBUG=True

abs_package_dir = os.path.join(os.path.abspath(
    os.path.dirname(__file__)), package_dir)

class my_build_ext(build_ext.build_ext):
    def run(self):
        if not self.dry_run: # honor the --dry-run flag
            build_dir = os.path.join(
                os.path.abspath(os.path.dirname(__file__)),
                'build')
            if not os.path.exists(build_dir): os.mkdir(build_dir)
            abs_build_dir = os.path.abspath(build_dir)
            pyx_obj = pyx2obj(wrapper_src_path, abs_build_dir,
                              metadir=abs_build_dir, cwd=package_dir)
            src_objs = compile_sources(srcs, abs_build_dir,
                                       metadir=abs_build_dir, cwd=package_dir)
            abs_so_path = link_py_so(src_objs+[pyx_obj], cwd=build_dir, fort=True)
            if self.inplace:
                copy(abs_so_path, abs_package_dir)
            else:
                print "only --inplace supported for now"
                raise NotImplementedError

setup(
    name='fastinverse',
    version='0.0.1',
    description='Python package using SymPy for generating fast C code solving inverse problems.',
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    url='https://github.com/bjodah/fastinverse',
    packages=['fastinverse'],
    cmdclass = {'build_ext': my_build_ext},
)
