#!/bin/bash -e
# Usage
#   $ ./scripts/run_tests.sh
# or
#   $ ./scripts/run_tests.sh --cov pycvodes --cov-report html
${PYTHON:-python3} setup.py build_ext -i
${PYTHON:-python3} -m pytest --doctest-modules --black --flake8 $@
${PYTHON:-python3} -m doctest README.rst
