#!/bin/bash
set -xeuo pipefail

PKG_NAME=${1:-${CI_REPO_NAME##*/}}

source /opt-3/cpython-v3.11-apt-deb/bin/activate

python3 setup.py sdist
#PKG_VERSION=$(python3 setup.py --version)
#(cd dist/; python3 -m pip install $PKG_NAME-$PKG_VERSION.tar.gz)
(cd dist/; python3 -m pip install $PKG_NAME-*.tar.gz)
(cd /; python3 -m pytest --pyargs $PKG_NAME)
CXX=clang++ CC=clang CFLAGS='-fsanitize=address' python3 -m pip install --force-reinstall .[all]
LD_PRELOAD=$(clang --print-file-name=libclang_rt.asan-$(uname -m).so) PYTHONMALLOC=malloc PYTHONPATH=$(pwd) ASAN_OPTIONS=detect_leaks=0,symbolize=1 ./scripts/run_tests.sh --cov $PKG_NAME --cov-report html
./scripts/coverage_badge.py htmlcov/ htmlcov/coverage.svg

# Make sure repo is pip installable from git-archive zip
git-archive-all --prefix='' /tmp/HEAD.zip
(
    cd /
    CFLAGS="-fopenmp -DFINITEDIFF_OPENMP" python3 -m pip install --force-reinstall /tmp/HEAD.zip
    python3 -c '
from finitediff import get_include as gi
import os
assert "finitediff_templated.hpp" in os.listdir(gi())
'
    python3 -m pytest --pyargs finitediff
)
