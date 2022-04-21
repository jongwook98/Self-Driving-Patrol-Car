#!/bin/bash

ROOT_DIR=$(git rev-parse --show-toplevel)
source "${ROOT_DIR}/scm/scripts/common/echo.sh"

echo_func "[scm] Delete unnecessary files"
sudo killall apt apt-get
sudo rm /var/lib/apt/lists/lock
sudo rm /var/cache/apt/archives/lock
sudo rm /var/lib/dpkg/lock*
sudo dpkg --configure -a

set -e

echo_func "[scm] Update and Upgrade the package"
sudo apt-get update && sudo apt-get upgrade -y

echo_func "[scm] Install the basic package"
sudo apt-get install -y build-essential \
		software-properties-common \
		python3-pip \
		python3-dev \
		python3-setuptools \
		pylint3 \
		gcc \
		git \
		make \
		clang-format \
		valgrind \
		shellcheck \
		cppcheck

echo_func "[scm] Install the Python package"
sudo pip3 install -U -r requirements.txt

sudo apt-get autoremove -y
sudo apt-get autoclean -y
sudo apt-get clean
echo_func "[scm] Complete to update/install all packages!"
