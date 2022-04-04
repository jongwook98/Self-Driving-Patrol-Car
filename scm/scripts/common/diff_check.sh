#!/bin/bash

DIFF=$(git diff)

if [ -z "${DIFF}" ]
then
	exit 0
fi

echo -e "\033[0;31m""[SCM ERR] Please correct the line below!""\033[0m"
git diff --exit-code
exit 1
