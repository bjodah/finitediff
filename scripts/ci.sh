#!/bin/bash -xeu
PKG_NAME=${1:-${CI_REPO##*/}}
if [[ "$CI_BRANCH" =~ ^v[0-9]+.[0-9]?* ]]; then
    eval export ${PKG_NAME^^}_RELEASE_VERSION=\$CI_BRANCH
    echo ${CI_BRANCH} | tail -c +2 > __conda_version__.txt
fi
python3 setup.py sdist
(cd dist/; python3 -m pip install $PKG_NAME-$($PYTHON ../setup.py --version).tar.gz)
(cd /; python3 -m pytest --pyargs $PKG_NAME)
python3 -m pip install --user -e .[all]
PYTHONPATH=$(pwd) ./scripts/run_tests.sh --cov $PKG_NAME --cov-report html
./scripts/coverage_badge.py htmlcov/ htmlcov/coverage.svg

# Make sure repo is pip installable from git-archive zip
git archive -o /tmp/archive.zip HEAD
(
    cd /
    python3 -m pip install --force-reinstall /tmp/archive.zip
    python3 -c '
from finitediff import get_include as gi
import os
assert "finitediff_templated.pxd" in os.listdir(gi())
'
)

./scripts/render_notebooks.sh
