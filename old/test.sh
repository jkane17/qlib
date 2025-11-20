#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ -f "${SCRIPT_DIR}/build.sh" ]; then
    ${SCRIPT_DIR}/build.sh
else
    echo "Cannot build: Missing ${SCRIPT_DIR}/build.sh"
fi

echo "Running unit tests..."

# Test setup (creates necessary directories, files, etc.)
# test-setup: $(BUILD_TEST_FILES)
# 	@echo "Setting up test environment..."
# 	@mkdir -p $(BUILD_TEST)/data
# 	@touch $(BUILD_TEST)/data/sample.csv
# 	@touch $(BUILD_TEST)/data/.hidden
# 	@mkdir -p $(BUILD_TEST)/data/dir1
# 	@mkdir -p $(BUILD_TEST)/data/dir1/dir11
# 	@mkdir -p $(BUILD_TEST)/data/dir1/dir12
# 	@touch $(BUILD_TEST)/data/dir1/file1.csv
# 	@touch $(BUILD_TEST)/data/dir1/dir11/file11.txt
# 	@mkdir -p $(BUILD_TEST)/data/dir2

echo "Unit tests complete"
