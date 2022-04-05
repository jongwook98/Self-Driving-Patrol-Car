#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

cd "${ROOT_DIR}/clang"
pwd

# static anaytics using cppcheck
cppcheck \
	--enable=style \
	-I include/ \
	-i out/ \
	--error-exitcode=1 \
	--verbose \
	--suppress=missingIncludeSystem \
	.

RET="$?"
if [[ "${RET}" != "0" ]]
then
	echo_func "[SCM ERR] Failed to pass the codes on the cppcheck" 1
	exit 1
fi

echo_func "[scm] Static Analytics CI test done!" 0
