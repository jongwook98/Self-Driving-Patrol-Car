#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

CORE_DIR="core"
LANE_DIR="features/lane_detection"

CORE_TARGET="sdpc"
LANE_TARGET="lane_detection"

function build()
{
	# change directory
	cd "${ROOT_DIR}/${1}"

	# build
	./build.sh

	# output check
	check_output "${2}"
}

function check_output()
{
	if [ ! -f "out/${1}" ]
	then
		echo_func "[SCM ERR] Not exist the output file" 1
		exit 1
	fi
}

# for core
build ${CORE_DIR} ${CORE_TARGET}
check_output "lib${CORE_TARGET}_shared.so"

# for lane detection
#build ${LANE_DIR} ${LANE_TARGET}

echo_func "[scm] Build CI test done!" 0
