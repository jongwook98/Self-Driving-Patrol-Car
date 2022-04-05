#!/bin/bash

SCRIPT_DIR="./scm/scripts"
CI_DIR="${SCRIPT_DIR}/ci"
#UNIT_DIR="${SCRIPT_DIR}/unittest"
ROOT_DIR=$(git rev-parse --show-toplevel)

set -e

cd "${ROOT_DIR}"

# general
bash ${CI_DIR}/general_ci.sh

# lang check
bash ${CI_DIR}/bash_ci.sh
bash ${CI_DIR}/clang_ci.sh
bash ${CI_DIR}/cmake_ci.sh
bash ${CI_DIR}/python_ci.sh

# static analytics
bash ${CI_DIR}/static_analytics.sh

# build
bash ${CI_DIR}/build_ci.sh

## unittest
#bash ${UNIT_DIR}/unittest_clang.sh
#bash ${UNIT_DIR}/unittest_python.sh
