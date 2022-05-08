#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
COMMON_DIR=${ROOT_DIR}/scm/scripts/common

source "${COMMON_DIR}/echo.sh"

cd "${ROOT_DIR}"
pwd

# check the clang-format using the .clang-format file
echo_func "[scm] clang-format checking" 0
find . \
	-type f \
	-not -path "${THIRD_PATH_PATH}" -a \
	-not -path "${OUT_PATH}" -a \
	-not -path "${MCU_CORE_PATH}" -a \
	-not -path "${MCU_DRIVER_PATH}"\
	\( -name "*.c" -o -name "*.h" \) \
	-exec clang-format -i {} \;

"${COMMON_DIR}/diff_check.sh"

RET="$?"
if [ "${RET}" -ne "0" ]
then
	exit 1
fi

echo_func "[scm] C language CI test done!" 0
