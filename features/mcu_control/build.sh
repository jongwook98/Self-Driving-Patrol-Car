#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

MCU_CONTROL_DIR=${ROOT_DIR}/features/mcu_control
OUT_DIR=${MCU_CONTROL_DIR}/out

CPU_NUM=$(grep -c processor /proc/cpuinfo)

function build_for_mcu()
{
	cd "${MCU_CONTROL_DIR}" || exit 1

	# build
	echo_func "[MCU] Build Start!" 0
	make -j "${CPU_NUM}" 2>&1 | tee build.log

	ret=$(grep -e " warning:" -e " error:" < build.log)
	if [[ ${ret} != "" ]]
	then
		echo_func "[MCU err] Occured the warning/error when compile time" 1
		rm -rf build.log
		exit 1
	fi

	rm -rf build.log
}

# remove out directory
rm -rf "${OUT_DIR}"

# build
build_for_mcu

echo_func "[MCU] Build Done!" 0
