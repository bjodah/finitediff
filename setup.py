#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import shutil

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


USE_CYTHON = os.path.exists('finitediff/_finitediff_templated.pyx')
ext = '.pyx' if USE_CYTHON else '.cpp'
sources += [
    './external/newton_interval/src/newton_interval.c',
    './finitediff/_finitediff_' + interface + ext
]

pkg_name = 'finitediff'

cmdclass = {}
ext_modules = []
if len(sys.argv) > 1 and '--help' not in sys.argv[1:] and sys.argv[1] not in (
            '--help-commands', 'egg_info', 'clean', '--version'):
    # e.g. egg_info must not import from dependencies (pycompilation)
    import numpy
    include_dirs = [
        './include',
        './external/newton_interval/include',
        numpy.get_include()
    ]

    if USE_FORTRAN:
        from pycompilation.dist import pc_build_ext, PCExtension
        from pycompilation.util import ArbitraryDepthGlob

        cmdclass = {'build_ext': pc_build_ext}
        ext_modules = [
            PCExtension(
                'finitediff._finitediff_'+interface,
                sources=sources,
                pycompilation_compile_kwargs={
                    'per_file_kwargs': {
                        ArbitraryDepthGlob(b'*.c'): {'std': 'c99'}
                    }
                },
                include_dirs=include_dirs,
                language=None if USE_FORTRAN else 'c++',
                logger=True
            )
        ]
    else:
        # default path (no external dependencies):
        from distutils.extension import Extension
        modname = '_finitediff_'+interface
        ext_modules = [
            Extension('finitediff.'+modname,
                      sources,
                      language='c++', extra_compile_args=['-std=c++11'],
                      include_dirs=include_dirs)
        ]
        if USE_CYTHON:
            from Cython.Build import cythonize
            ext_modules = cythonize(ext_modules, include_path=include_dirs,
                                    gdb_debug=True)

FINITEDIFF_RELEASE_VERSION = os.environ.get('FINITEDIFF_RELEASE_VERSION', '')

# http://conda.pydata.org/docs/build.html#environment-variables-set-during-the-build-process
CONDA_BUILD = os.environ.get('CONDA_BUILD', '0') == '1'
if CONDA_BUILD:
    try:
        FINITEDIFF_RELEASE_VERSION = 'v' + open(
            '__conda_version__.txt', 'rt').readline().rstrip()
    except IOError:
        pass

release_py_path = os.path.join(pkg_name, 'release.py')

if len(FINITEDIFF_RELEASE_VERSION) > 1 and \
   FINITEDIFF_RELEASE_VERSION[0] == 'v':
    TAGGED_RELEASE = True
    __version__ = FINITEDIFF_RELEASE_VERSION[1:]
else:
    TAGGED_RELEASE = False
    # read __version__ attribute from release.py:
    exec(open(release_py_path).read())

classifiers = [
    "Development Status :: 4 - Beta",
    'License :: OSI Approved :: BSD License',
    'Operating System :: OS Independent',
    'Topic :: Scientific/Engineering',
    'Topic :: Scientific/Engineering :: Mathematics',
]


setup_kwargs = dict(
    name=pkg_name,
    version=__version__,  # from release_py_path
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    description=('Finite difference weights for any derivative '
                 'order on arbitrarily spaced grids.'),
    classifiers=classifiers,
    url='https://github.com/bjodah/'+pkg_name,
    download_url=('https://github.com/bjodah/' + pkg_name +
                  '/archive/v'+__version__+'.tar.gz'),
    packages=[pkg_name],
    cmdclass=cmdclass,
    ext_modules=ext_modules
)

if __name__ == '__main__':
    try:
        if TAGGED_RELEASE:
            # Same commit should generate different sdist
            # depending on tagged version (set FINITEDIFF_RELEASE_VERSION)
            # this will ensure source distributions contain the correct version
            shutil.move(release_py_path, release_py_path+'__temp__')
            open(release_py_path, 'wt').write(
                "__version__ = '{}'\n".format(__version__))
        setup(**setup_kwargs)
    finally:
        if TAGGED_RELEASE:
            shutil.move(release_py_path+'__temp__', release_py_path)
