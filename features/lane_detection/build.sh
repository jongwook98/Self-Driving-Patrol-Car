#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

OUT_DIR="out"
CPU_NUM=$(grep -c processor /proc/cpuinfo)
LOG="build.log"

function check_error()
{
	ret=$(grep -e " warning:" -e " error:" < ${LOG})
	if [[ ${ret} != "" ]]
	then
		echo_func "[clang err] Occured the warning/error when compile time" 1
		rm -rf ${LOG}
		exit 1
	fi

	rm -rf ${LOG}
}

function build_for_glog()
{
	cmake \
		-S "${ROOT_DIR}/third_party/glog" \
		-B "${OUT_DIR}/glog" \
		-G "Unix Makefiles"

	cd "${OUT_DIR}/glog" || exit 1

	# build
	echo_func "[features/lane_detection] Third Party App Build  Start!" 0
	make -j "${CPU_NUM}" 2>&1 | tee ${LOG}
	check_error

	cd - || exit 1
}

function build_for_lane_detection()
{
	cd "${OUT_DIR}" || exit 1

	# build
	echo_func "[features/lane_detection] Build Start!" 0
	cmake ../
	make -j "${CPU_NUM}" 2>&1 | tee ${LOG}
	check_error

	cd - || exit 1
}

rm -rf ${OUT_DIR}
mkdir -p ${OUT_DIR}

build_for_glog
build_for_lane_detection

echo_func "[features/lane_detection] Build Done!" 0
