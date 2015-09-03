#!/bin/bash -xe
if [[ $1 != v* ]]; then
    echo "Argument does not start with 'v'"
    exit 1
fi
cd $(dirname $0)/..
# PKG will be name of the directory one level up containing "__init__.py" 
PKG=$(find . -maxdepth 2 -name __init__.py -print0 | xargs -0 -n1 dirname | xargs basename)
PKG_UPPER=$(echo $PKG | tr '[:lower:]' '[:upper:]')
./scripts/run_tests.sh
env ${PKG_UPPER}_RELEASE_VERSION=$1 python setup.py sdist
echo $1>__conda_version__.txt
trap "rm __conda_version__.txt" EXIT SIGINT SIGTERM
conda build conda-recipe

# All went well

git tag -a $1 -m $1
git push --tags
twine upload dist/${PKG}-${1#v}.tar.gz
echo "Remember to bump dev version!"
