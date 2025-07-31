#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

BUILD_DIR=${SCRIPT_DIR}/build
CONFIG_DIR=${SCRIPT_DIR}/config
SRC_DIR=${SCRIPT_DIR}/src

BUILD_CONFIG_DIR=${BUILD_DIR}/config
BUILD_LIB_DIR=${BUILD_DIR}/lib

echo "Building..."

mkdir -p ${BUILD_DIR}
mkdir -p ${BUILD_CONFIG_DIR}
mkdir -p ${BUILD_LIB_DIR}

cp -r ${CONFIG_DIR}/* ${BUILD_CONFIG_DIR}
cp -r ${SRC_DIR}/* ${BUILD_LIB_DIR}

echo "Build complete"
