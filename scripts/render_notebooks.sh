#!/bin/bash -e
QUIET_EXIT_CODE=0
function quiet_unless_fail {
    # suppresses function output unless exit status is != 0
    OUTPUT_FILE=$(tempfile)
    #/bin/rm --force /tmp/suppress.out 2>/dev/null
    EXECMD=${1+"$@"}
    $EXECMD > ${OUTPUT_FILE} 2>&1
    QUIET_EXIT_CODE=$?
    if [ ${QUIET_EXIT_CODE} -ne 0 ]; then
	cat ${OUTPUT_FILE}
	echo "The following command exited with exit status ${QUIET_EXIT_CODE}: ${EXECMD}"
    fi
    /bin/rm ${OUTPUT_FILE}
}


cd examples/

set -x
for ipynb in *.ipynb; do
    #quiet_unless_fail
    jupyter nbconvert --log-level=INFO --to=html --ExecutePreprocessor.enabled=True --ExecutePreprocessor.timeout=900 "${ipynb}"
    #if [ ${QUIET_EXIT_CODE} -ne 0 ]; then
    #    exit ${QUIET_EXIT_CODE}
    #fi
done
#../scripts/render_index.sh *.html
