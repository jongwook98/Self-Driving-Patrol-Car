#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
COMMON_DIR=${ROOT_DIR}/scm/scripts/common
LINT_LOG="cmake_lint.log"

source "${COMMON_DIR}/echo.sh"

cd "${ROOT_DIR}"
pwd

# CI using the cmakelint
find . \
	-not -path "${THIRD_PARTY_PATH}" \
	-name "CMakeLists.txt" \
	-exec cmakelint --spaces=2 --linelength=80 {} \; 2>&1 | tee ${LINT_LOG}

# Check the error
#ERRORS=$(cat ${LINT_LOG} | grep "Total Errors" | awk '{print $3}')
ERRORS=$(grep "Total Errors" < ${LINT_LOG} | awk '{print $3}')
for NUM in ${ERRORS}
do
	if [[ "${NUM}" != "0" ]]
	then
		echo_func "[SCM ERR] Need to check the CMakeLists.txt" 1
		exit 1
	fi
done

rm ${LINT_LOG}

echo_func "[scm] CMakeLists.txt CI test done!" 0
