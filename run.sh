#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)
COMMON_DIR="${ROOT_DIR}/scm/scripts/common"

CORE_DIR="${ROOT_DIR}/core"
CORE_BIN="sdpc"

LANE_DETECTION_DIR="${ROOT_DIR}/features/lane_detection"
LANE_DETECTION_BIN="lane_detection"

function kill_process()
{
	while true
	do
		KILL_PID=$(pgrep -o "${1}")
		if [[ ${KILL_PID} == "" ]]
		then
			break
		fi

		kill -SIGINT "${KILL_PID}"
		sleep 1
	done
}

function cleanup()
{
	set +e
	echo_func "[+] Try to the cleanup the run.sh." 0

	kill_process "${LANE_DETECTION_BIN}"
	kill_process "${CORE_BIN}"

	chown -R "${SUDO_USER}":"${SUDO_USER}" "${CORE_DIR}/out"
	chown -R "${SUDO_USER}":"${SUDO_USER}" "${LANE_DETECTION_DIR}/out"

	echo_func "[+] Done." 0
	exit 0
}
trap cleanup SIGHUP SIGINT SIGQUIT SIGABRT SIGALRM SIGTERM

set -e
source "${COMMON_DIR}/echo.sh"

# check the root user
if [[ $(id -u) -ne 0 ]]
then
	echo_func "[-] Please run as the root!" 1
	exit 1
fi

# execute core as the background
cd "${CORE_DIR}" && ./build.sh && cd out
"./${CORE_BIN}" &

# execute feature/lane_detection as the background
cd "${LANE_DETECTION_DIR}" && ./build.sh && cd out
"./${LANE_DETECTION_BIN}" &

while true
do
	echo_func "[+] Run the self-driving-patrol-car,," 0
	sleep 1
done

echo_func "[-] Not reached,," 1
