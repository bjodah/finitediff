#!/bin/bash -xeu
PKG_NAME=${1:-${DRONE_REPO##*/}}
if [[ "$DRONE_BRANCH" =~ ^v[0-9]+.[0-9]?* ]]; then
    eval export ${PKG_NAME^^}_RELEASE_VERSION=\$DRONE_BRANCH
    echo ${DRONE_BRANCH} | tail -c +2 > __conda_version__.txt
fi
python3 setup.py sdist
PKG_VERSION=$(python3 setup.py --version)
(cd dist/; python3 -m pip install $PKG_NAME-$PKG_VERSION.tar.gz)
(cd /; python3 -m pytest --pyargs $PKG_NAME)
CXX=clang++-10 CC=clang-10 CFLAGS='-fsanitize=address' python3 -m pip install --force-reinstall .[all]
LD_PRELOAD=/usr/lib/llvm-10/lib/clang/10.0.0/lib/linux/libclang_rt.asan-x86_64.so PYTHONMALLOC=malloc PYTHONPATH=$(pwd) ASAN_OPTIONS=detect_leaks=0,symbolize=1 ./scripts/run_tests.sh --cov $PKG_NAME --cov-report html
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
