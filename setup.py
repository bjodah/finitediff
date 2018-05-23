#!/usr/bin/env python
# -*- coding: utf-8 -*-

import io
import os
import pprint
import re
import shutil
import subprocess
import sys
import warnings

from setuptools import setup

pkg_name = 'finitediff'
url = 'https://github.com/bjodah/' + pkg_name
license = 'BSD'


def _path_under_setup(*args):
    return os.path.join(os.path.dirname(__file__), *args)


release_py_path = _path_under_setup(pkg_name, '_release.py')
config_py_path = _path_under_setup(pkg_name, '_config.py')
env = None  # silence pyflakes, 'env' is actually set on the next line
exec(open(config_py_path).read())
for k, v in list(env.items()):
    env[k] = os.environ.get('%s_%s' % (pkg_name.upper(), k), v)

_version_env_var = '%s_RELEASE_VERSION' % pkg_name.upper()
RELEASE_VERSION = os.environ.get(_version_env_var, '')

# http://conda.pydata.org/docs/build.html#environment-variables-set-during-the-build-process
CONDA_BUILD = os.environ.get('CONDA_BUILD', '0') == '1'
if CONDA_BUILD:
    try:
        RELEASE_VERSION = 'v' + open(
            '__conda_version__.txt', 'rt').readline().rstrip()
    except IOError:
        pass


if len(RELEASE_VERSION) > 1 and RELEASE_VERSION[0] == 'v':
    TAGGED_RELEASE = True
    __version__ = RELEASE_VERSION[1:]
else:
    TAGGED_RELEASE = False
    # read __version__ attribute from _release.py:
    exec(io.open(release_py_path, encoding='utf-8').read())
    if __version__.endswith('git'):
        try:
            _git_version = subprocess.check_output(
                ['git', 'describe', '--dirty']).rstrip().decode('utf-8').replace('-dirty', '.dirty')
        except subprocess.CalledProcessError:
            warnings.warn("A git-archive is being installed - version information incomplete.")
        else:
            if 'develop' not in sys.argv:
                warnings.warn("Using git to derive version: dev-branches may compete.")
                __version__ = re.sub('v([0-9.]+)-(\d+)-(\w+)', r'\1.post\2+\3', _git_version)  # .dev < '' < .post


interface = 'c'
other_sources = [os.path.join('src', 'finitediff_c.c')]

basename = '_finitediff_'+interface
ext = '.c'
if os.path.exists(_path_under_setup('finitediff', basename+ext)):
    USE_CYTHON = False
else:
    USE_CYTHON = True
    ext = '.pyx'

modname = 'finitediff.' + basename
srcname = os.path.join('finitediff', basename)
other_sources += [
    os.path.join('finitediff', 'external', 'newton_interval', 'src', 'newton_interval.c')
]

cmdclass = {}
ext_modules = []
if len(sys.argv) > 1 and '--help' not in sys.argv[1:] and not any(arg in (
        '--help-commands', 'egg_info', 'clean', '--version') for arg in sys.argv[1:]):
    # e.g. egg_info must not import from dependencies (pycompilation)
    import numpy
    include_dirs = [
        'finitediff/external/newton_interval/include',
        'finitediff/include',
        numpy.get_include()
    ]

    # default path (no external dependencies):
    from setuptools.extension import Extension
    ext_modules = [
        Extension(modname, [srcname+ext], include_dirs=include_dirs)
    ]
    if USE_CYTHON:
        from Cython.Build import cythonize
        ext_modules = cythonize(ext_modules, include_path=include_dirs,
                                compiler_directives={'embedsignature': True, 'binding': True})
    else:
        ext_modules[0].sources += other_sources

    if ext_modules[0].sources[0].startswith('/'):
        raise ValueError("Absolute path not allowed: %s" % ext_modules[0].sources[0])

submodules = [
    pkg_name + '.grid',
]

tests = [
    pkg_name + '.tests',
    pkg_name + '.grid.tests',
]

classifiers = [
    "Development Status :: 4 - Beta",
    'License :: OSI Approved :: BSD License',
    'Operating System :: OS Independent',
    'Topic :: Scientific/Engineering',
    'Topic :: Scientific/Engineering :: Mathematics',
]

with io.open(_path_under_setup(pkg_name, '__init__.py'), 'rt', encoding='utf-8') as f:
    short_description = f.read().split('"""')[1].split('\n')[1]
if not 10 < len(short_description) < 255:
    warnings.warn("Short description from __init__.py proably not read correctly.")
long_description = io.open(_path_under_setup('README.rst'),
                           encoding='utf-8').read()
if not len(long_description) > 100:
    warnings.warn("Long description from README.rst probably not read correctly.")
_author, _author_email = io.open(_path_under_setup('AUTHORS'), 'rt', encoding='utf-8').readline().split('<')


setup_kwargs = dict(
    name=pkg_name,
    version=__version__,  # from release_py_path
    description=short_description,
    long_description=long_description,
    author=_author.strip(),
    author_email=_author_email.split('>')[0].strip(),
    url=url,
    license=license,
    keywords=["finite-difference", "taylor series", "extrapolation", "interpolation"],
    packages=[pkg_name] + submodules + tests,
    include_package_data=True,
    cmdclass=cmdclass,
    ext_modules=ext_modules,
    classifiers=classifiers,
    setup_requires=['numpy'] + (['cython'] if USE_CYTHON else []),
    install_requires=['numpy'],
    extras_require={'all': ['scipy', 'pytest', 'sphinx', 'sphinx_rtd_theme', 'numpydoc']}
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
        shutil.move(config_py_path, config_py_path+'__temp__')
        with open(config_py_path, 'wt') as fh:
            fh.write("env = {}\n".format(pprint.pformat(env)))
        setup(**setup_kwargs)
    finally:
        if TAGGED_RELEASE:
            shutil.move(release_py_path+'__temp__', release_py_path)
        shutil.move(config_py_path+'__temp__', config_py_path)
