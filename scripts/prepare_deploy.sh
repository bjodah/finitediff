#!/bin/bash
cp LICENSE doc/_build/html/
mkdir -p deploy/public_html/branches/"${CI_BRANCH}"
cp -r dist/* htmlcov/ examples/ doc/_build/html/ deploy/public_html/branches/"${CI_BRANCH}"/
