#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

function check_static_annalytics()
{
	# static anaytics using cppcheck
	cppcheck \
		--enable=style \
		-I include/ \
		-i out/ \
		-i Core/ \
		-i Drivers/ \
		-i matlab/ \
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
}

# check in core
cd "${ROOT_DIR}/core"
pwd
check_static_annalytics

# check in lane detection & mcu control
cd "${ROOT_DIR}/features"
pwd
check_static_annalytics

echo_func "[scm] Static Analytics CI test done!" 0
