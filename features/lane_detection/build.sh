#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

OUT_DIR="out"
CPU_NUM=$(grep -c processor /proc/cpuinfo)

function build_for_lane_detection()
{
	cd "${OUT_DIR}" || exit 1

	# build
	echo_func "[features/lane_detection] Build Start!" 0
	cmake ../
	make -j "${CPU_NUM}" 2>&1 | tee build.log

	ret=$(grep -e " warning:" -e " error:" < build.log)
	if [[ ${ret} != "" ]]
	then
		echo_func "[clang err] Occured the warning/error when compile time" 1
		rm -rf build.log
		exit 1
	fi

	rm -rf build.log
}

rm -rf ${OUT_DIR}
mkdir -p ${OUT_DIR}

build_for_lane_detection

echo_func "[features/lane_detection] Build Done!" 0
