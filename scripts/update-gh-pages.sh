#!/bin/bash -x
#
# Usage:
#
#    $ ./scripts/update-gh-pages.sh v0.6.0 origin
#

tag=${1:-master}
remote=${2:-origin}

ori_branch=$(git rev-parse --symbolic-full-name --abbrev-ref HEAD)
tmpdir=$(mktemp -d)

cp -r doc/_build/html/ $tmpdir
git ls-files --others | tar cf $tmpdir/untracked.tar -T -
cleanup() {
    git checkout -f $ori_branch
    tar xf $tmpdir/untracked.tar
    rm -r $tmpdir
}
trap cleanup INT TERM EXIT

if [[ -d .gh-pages-skeleton ]]; then
    cp -r .gh-pages-skeleton $tmpdir
fi

git fetch $remote
git checkout gh-pages
if [[ $? -ne 0 ]]; then
    git checkout --orphan gh-pages
    if [[ $? -ne 0 ]]; then
        >&2 echo "Failed to switch to 'gh-pages' branch."
        exit 1
    fi
    preexisting=0
else
    preexisting=1
    git pull
    if [[ $? -ne 0 ]]; then
        >&2 echo "git pull failed (no internet connection?)"
        exit 1
    fi
fi

if [[ $preexisting == 1 ]]; then
    while [[ "$(git log -1 --pretty=%B)" == Volatile* ]]; do
        # overwrite previous docs
        git reset --hard HEAD~1
    done
else
    git reset --hard
fi

git clean -xfd
if [[ $preexisting == 1 ]]; then
    mv v*/ $tmpdir
    git rm -rf * > /dev/null
fi
cp -r $tmpdir/html/ $tag
if [[ $preexisting == 1 ]]; then
    mv $tmpdir/v*/ .
fi
if [[ -d $tmpdir/.gh-pages-skeleton ]]; then
    cp -r $tmpdir/.gh-pages-skeleton/. .
fi
if [[ "$tag" == v* ]]; then
    ln -s $tag latest
    commit_msg="Release docs for $tag"
else
    if [[ $preexisting == 1 ]]; then
        commit_msg="Volatile ($tag) docs"
    else
        commit_msg="Initial commit"
    fi
fi
git add -f . >/dev/null
git commit -m "$commit_msg"
if [[ $preexisting == 1 ]]; then
    git push -f $remote gh-pages
else
    git push --set-upstream $remote gh-pages
fi
