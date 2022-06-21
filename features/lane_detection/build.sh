#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

OUT_DIR="out"
CPU_NUM=$(grep -c processor /proc/cpuinfo)
LOG="build.log"

GOOGLE_LOG="glog"
MQTT="mosquitto"

function check_error()
{
	ret=$(grep -e " warning:" -e " error:" < ${LOG})
	if [[ ${ret} != "" ]]
	then
		echo_func "[ERR] Occured the warning/error when compile time" 1
		rm -rf ${LOG}
		exit 1
	fi

	rm -rf ${LOG}
}

function check_mosquitto_daemon()
{
	mqtt_status=$(systemctl status ${MQTT} | grep "active (running)")
	while [[ ${mqtt_status} == "" ]]
	do
		systemctl restart ${MQTT}
		echo_func "[features/lane_detection] Starting the ${MQTT},,,"
		sleep 1

		mqtt_status=$(systemctl status ${MQTT} | grep "active (running)")
	done
}

function build_for_third_party_app()
{
	cmake \
		-S "${ROOT_DIR}/third_party/${1}" \
		-B "${OUT_DIR}/${1}" \
		-G "Unix Makefiles"

	cd "${OUT_DIR}/${1}" || exit 1

	# build
	echo_func "[features/lane_detection] Third Party App ${1} Build Start!" 0
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

build_for_third_party_app ${GOOGLE_LOG}
build_for_third_party_app ${MQTT}

build_for_lane_detection

check_mosquitto_daemon

echo_func "[features/lane_detection] Build Done!" 0
