#!/bin/bash

BACK_CR="\033[0m"
RED_CR="\033[0;31m"
GREEN_CR="\033[0;32m"

export THIRD_PATH_PATH="*/third_party/*"
export OUT_PATH="*/out/*"
export IMAGE_PATH="*/image/*"
export MCU_CORE_PATH="*/Core/*"
export MCU_DRIVER_PATH="*/Drivers/*"
export MCU_MATALB_PATH="*/matlab/*"

echo_func()
{
	if [[ $2 -eq 1 ]]
	then
		echo -e "${RED_CR}${1}${BACK_CR}"
	else
		echo -e "${GREEN_CR}${1}${BACK_CR}"
	fi
}
