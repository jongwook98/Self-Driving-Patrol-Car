#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
COMMON_DIR=${ROOT_DIR}/scm/scripts/common

source "${COMMON_DIR}/echo.sh"


cd "${ROOT_DIR}"
pwd

# print the basic informations
echo_func "[scm] OS version" 0
grep "VERSION=" < "/etc/os-release"

echo_func "[scm] Kernel version" 0
uname -a

echo_func "[scm] All the git branches" 0
git branch -a

# remove trailing lines
echo_func "[scm] Remove the trailing lines" 0
find . \
	-type f \
	-not -path './.git/*' -a \
	-not -path "${THIRD_PATH_PATH}" -a \
	-not -path "${OUT_PATH}" -a \
	-not -path "${IMAGE_PATH}" -a \
	-not -path "${MCU_CORE_PATH}" -a \
	-not -path "${MCU_DRIVER_PATH}" -a \
	-not -path "${MCU_MATALB_PATH}" \
	-exec sed -i '${/^$/d;}' {} \;

# trim whitespcae
echo_func "[scm] Trim whitespace" 0
find . \
	-type f \
	-not -path './.git/*' -a \
	-not -path "${THIRD_PATH_PATH}" -a \
	-not -path "${OUT_PATH}" -a \
	-not -path "${IMAGE_PATH}" -a \
	-not -path "${MCU_CORE_PATH}" -a \
	-not -path "${MCU_DRIVER_PATH}" -a \
	-not -path "${MCU_MATALB_PATH}" \
	-exec sed -i 's/[[:space:]]*$//' {} \;

"${COMMON_DIR}/diff_check.sh"

RET="$?"
if [ "${RET}" -ne "0" ]
then
	exit 1
fi

echo_func "[scm] General CI test done!" 0
