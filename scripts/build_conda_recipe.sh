#!/bin/bash -ex
# Usage:
#
#    $ ./scripts/build_conda_recipe.sh v1.2.3
#
if [[ $1 != v* ]]; then
    echo "Argument does not start with 'v'"
    exit 1
fi
./scripts/check_clean_repo_on_master.sh
echo ${1#v}>__conda_version__.txt
trap "rm __conda_version__.txt" EXIT INT TERM
for CPY in {27,34}; do
    CONDA_PY=$CPY conda build conda-recipe
done
