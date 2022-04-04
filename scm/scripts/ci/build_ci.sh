#!/bin/bash

set -e

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

TARGET="mpc"

# git pull the submodule
cd "${ROOT_DIR}"
git config --global http.sslVerify false
git submodule update --init

# set up the environment to build
cd "${ROOT_DIR}/clang"
rm -rf out
mkdir out

# build
./build.sh

# check the output file
if [ ! -f "out/${TARGET}" ] || [ ! -f "out/lib${TARGET}_shared.so" ]
then
	echo_func "[SCM ERR] Not exist the output file" 1
	exit 1
fi

echo_func "[scm] Build CI test done!" 0
