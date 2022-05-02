#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
COMMON_DIR=${ROOT_DIR}/scm/scripts/common

source "${COMMON_DIR}/echo.sh"

cd "${ROOT_DIR}"
pwd

# check to use the cpplint
echo_func "[scm] cpplint checking" 0

FILES=$(find . \
	-type f \
	-not -path '*/out/*' \
	-not -path '*/core/*' \
	\( -name "*.cpp" -o -name "*.h" \)
)

for file in ${FILES}
do
	cpplint "${file}"

	RET="$?"
	if [ "${RET}" -ne "0" ]
	then
		echo_func "[SCM ERR] Need to check the ${file}." 1
		exit 1
	fi
done

echo_func "[scm] C++ language CI test done!" 0
