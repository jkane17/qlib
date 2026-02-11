#!/bin/bash

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR=${PROJECT_DIR}/src

usage() {
    echo "Usage: $0 [-d DIRECTORY] [-h]"
    echo "  -d DIRECTORY    Set custom install directory"
    echo "  -h              Show this help message and exit"
}

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d)
            if [ -z "$2" ]; then
                echo "Error: -d requires a directory argument"
                usage
                exit 1
            fi
            INSTALL_DIR="$2"
            shift 2
            ;;
        -h)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# If INSTALL_DIR not provided via -d, use default logic
if [ -z "${INSTALL_DIR}" ]; then
    if [ -z "${QHOME}" ]; then
        INSTALL_DIR="${HOME}/.kx/mod/qlib/"
    else
        INSTALL_DIR="${QHOME}/mod/qlib/"
    fi
fi

# Create and prepare install directory
mkdir -p "${INSTALL_DIR}" || { echo "Error: Failed to create directory ${INSTALL_DIR}"; exit 1; }
rm -rf "${INSTALL_DIR}"/* || { echo "Error: Failed to clean directory ${INSTALL_DIR}"; exit 1; }
cp "${SRC_DIR}"/* "${INSTALL_DIR}/" || { echo "Error: Failed to copy files to ${INSTALL_DIR}"; exit 1; }

echo "Installation complete: ${INSTALL_DIR}"
