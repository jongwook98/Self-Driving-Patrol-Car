#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

export ENV SHELLCHECK_OPTS="--shell=bash --exclude=SC1090"

cd "${ROOT_DIR}"
pwd

BASH_LISTS=($(find . \
	-not -path '*/third_party/*' \
	-not -path './gentags.sh' \
	-name "*.sh"
))

for SCRIPT in "${BASH_LISTS[@]}"
do
	echo "Checking the ${SCRIPT}"
	shellcheck -x "${SCRIPT}"

	RET="$?"
	if [[ "${RET}" != "0" ]]
	then
		echo_func "[SCM ERR] Need to check the ${SCRIPT} file"
		exit 1
	fi
done

echo_func "[scm] Bash scripts CI test done!" 0
