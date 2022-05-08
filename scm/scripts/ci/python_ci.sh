#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
COMMON_DIR=${ROOT_DIR}/scm/scripts/common

source "${COMMON_DIR}/echo.sh"

cd "${ROOT_DIR}"
pwd

# check to exist the python files
py_files=$(find . \
	-type f \
	-not -path '*/.git/*' -a \
	-not -path "${THIRD_PATH_PATH}" -a \
	-not -path "${OUT_PATH}" -a \
	-not -path "${MCU_CORE_PATH}" -a \
	-not -path "${MCU_DRIVER_PATH}" \
	-name '*.py')
if [[ "${py_files}" == "" ]]
then
	echo_func "[SCM ERR] Not exist the python files!" 1
	exit 0
fi

# check the coding style score
for py_file in ${py_files}
do
	echo_func "[scm] Start the checking coding style about the ${py_file}" 0

	# sort the import
	python3 -m isort -y "${py_file}"
	echo_func "[scm] Completed to check the \"isort\"" 0

	# coding style check focused on the seeing
	python3 -m yapf -ir "${py_file}"
	echo_func "[scm] Completed to check the \"yapf\""

	"${COMMON_DIR}/diff_check.sh"

	RET="$?"
	if [ "${RET}" -ne "0" ]
	then
		exit 1
	fi

	# coding style check
	python3 -m flake8 "${py_file}"

	RET="$?"
	if [ "${RET}" -ne "0" ]
	then
		exit 1
	fi
	echo_func "[scm] Completed to check the \"flake8\""

	lint_result=$(pylint3 "${py_file}" 2>&1 | \
		grep "Your code" | \
		awk '{print $7}' | \
		awk -F '/' '{print $1}')
	if [[ "${lint_result}" != "10.00" ]]
	then
		echo_func "[SCM ERR] pylint3 result is ${lint_result}" 1
		pylint3 "${py_file}"
		exit 1
	fi
	echo_func "[scm] Completed to check the \"pylint\""
	echo
done

echo_func "[scm] Python CI test done!" 0
