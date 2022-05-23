#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

CORE_DIR=${ROOT_DIR}/core
OUT_DIR=${CORE_DIR}/out

CPU_NUM=$(grep -c processor /proc/cpuinfo)

# check the output directory
function create_dir()
{
	if [ ! -d "$1" ]
	then
		echo_func "[clang] Created the directory $1" 0
		mkdir "$1"
	fi
}

function build_for_clang()
{
	cd "${OUT_DIR}" || exit 1

	# build
	echo_func "[clang] Build Start!" 0
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

# remove out directory
rm -rf "${OUT_DIR}"

# check the output directory
create_dir "${OUT_DIR}"

# build the clang
build_for_clang

echo_func "[clang] Build Done!" 0
