#!/bin/bash -e
cd $(dirname $0)/..
python2 setup.py build_ext -i
python2 -m pytest --pep8 --flakes --ignore build/
python3 setup.py build_ext -i
python3 -m pytest
