#!/bin/bash -xeu
# Usage:
#
#    $ ./scripts/release.sh v1.2.3 ~/anaconda2/bin myserver.example.com
#

if [[ $1 != v* ]]; then
    echo "Argument does not start with 'v'"
    exit 1
fi
VERSION=${1#v}
CONDA_PATH=$2
SERVER=$3
find . -type f -iname "*.pyc" -exec rm {} +
find . -type f -iname "*.o" -exec rm {} +
find . -type f -iname "*.so" -exec rm {} +
find . -type d -name "__pycache__" -exec rmdir {} +
./scripts/check_clean_repo_on_master.sh
cd $(dirname $0)/..
# PKG will be name of the directory one level up containing "__init__.py" 
PKG=$(find . -maxdepth 2 -name __init__.py -print0 | xargs -0 -n1 dirname | xargs basename)
! grep --include "*.py" "will_be_missing_in='$VERSION'" -R $PKG/  # see deprecation()
PKG_UPPER=$(echo $PKG | tr '[:lower:]' '[:upper:]')
./scripts/run_tests.sh
env ${PKG_UPPER}_RELEASE_VERSION=v$VERSION python setup.py sdist
env ${PKG_UPPER}_RELEASE_VERSION=v$VERSION ./scripts/generate_docs.sh
# for CONDA_PY in 2.7 3.4 3.5; do
#     for CONDA_NPY in 1.11; do
#         PATH=$CONDA_PATH:$PATH ./scripts/build_conda_recipe.sh v$VERSION --python $CONDA_PY --numpy $CONDA_NPY
#     done
# done

# All went well, add a tag and push it.
git tag -a v$VERSION -m v$VERSION
git push
git push --tags
env ${PKG_UPPER}_RELEASE_VERSION=v$VERSION python setup.py upload_sphinx
twine upload dist/${PKG}-$VERSION.tar.gz
MD5=$(md5sum dist/${PKG}-$VERSION.tar.gz | cut -f1 -d' ')
cp -r conda-recipe/ dist/conda-recipe-$VERSION
sed -i -E \
    -e "s/version:(.+)/version: $VERSION/" \
    -e "s/path:(.+)/fn: $PKG-$VERSION.tar.gz\n  url: https:\/\/pypi.python.org\/packages\/source\/${PKG:0:1}\/$PKG\/$PKG-$VERSION.tar.gz#md5=$MD5\n  md5: $MD5/" \
    -e "/cython/d" \
    dist/conda-recipe-$VERSION/meta.yaml

# Specific for this project:
scp -r dist/conda-recipe-$VERSION/ $PKG@$SERVER:~/public_html/conda-recipes/
scp dist/${PKG}-$VERSION.tar.gz $PKG@$SERVER:~/public_html/releases/
for CONDA_PY in 2.7 3.4 3.5; do
    for CONDA_NPY in 1.11; do
        ssh $PKG@$SERVER "source /etc/profile; conda-build --python $CONDA_PY --numpy $CONDA_NPY ~/public_html/conda-recipes/conda-recipe-$VERSION/"
    done
done
