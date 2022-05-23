#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)

set -e

function process_clean() 
{
	echo "process_clean!"

    pkill -SIGINT sdpc
    pkill -SIGINT lane_detection

    if [ -e /dev/shm/* ] || [ -c /dev/mqueue/* ]
    then
        sudo rm -rf /dev/shm/*  
        sudo rm -rf /dev/mqueue/*
    else
        echo "clean successfully!"
    fi

	exit 1
}

cd ${ROOT_DIR}/core

./build.sh
sudo ./out/sdpc &
#SDPC_PID=$!

cd ${ROOT_DIR}/features/lane_detection

./build.sh
sudo ./out/lane_detection &
#LANE_PID=$!

trap "process_clean" SIGINT

while true
do
	sleep 1
done
