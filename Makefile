
# Directories
SRC_DIR := src
CNF_DIR := cnf
TEST_DIR := test
BUILD_DIR := build
BUILD_SRC := $(BUILD_DIR)/$(SRC_DIR)
BUILD_CNF := $(BUILD_DIR)/$(CNF_DIR)
BUILD_TEST := $(BUILD_DIR)/$(TEST_DIR)

# Find all Q files in the src and test directories, including subdirectories
SRC_FILES := $(shell find $(SRC_DIR) -name '*.q')
TEST_FILES := $(shell find $(TEST_DIR) -name '*.q')
BUILD_SRC_FILES := $(SRC_FILES:$(SRC_DIR)/%=$(BUILD_SRC)/%)
BUILD_TEST_FILES := $(TEST_FILES:$(TEST_DIR)/%=$(BUILD_TEST)/%)

# Default target
all: setup build

# Setup build directory
setup:
	@echo "Creating build directory..."
	mkdir -p $(BUILD_DIR)

build: prune $(BUILD_SRC_FILES) build-cnf
	@echo "Building complete."

$(BUILD_SRC)/%: $(SRC_DIR)/%
	@echo "Copying $< to $@"
	@mkdir -p $(dir $@)
	@cp $< $@

build-cnf:
	@echo "Building config files..."
	@mkdir -p $(BUILD_CNF)
	@cp -r $(CNF_DIR)/* $(BUILD_CNF)

# Test setup (creates necessary directories, files, etc.)
test-setup: $(BUILD_TEST_FILES)
	@echo "Setting up test environment..."
	@mkdir -p $(BUILD_TEST)/data
	@touch $(BUILD_TEST)/data/sample.csv
	@touch $(BUILD_TEST)/data/.hidden
	@mkdir -p $(BUILD_TEST)/data/dir1
	@mkdir -p $(BUILD_TEST)/data/dir1/dir11
	@mkdir -p $(BUILD_TEST)/data/dir1/dir12
	@touch $(BUILD_TEST)/data/dir1/file1.csv
	@touch $(BUILD_TEST)/data/dir1/dir11/file11.txt
	@mkdir -p $(BUILD_TEST)/data/dir2

$(BUILD_TEST)/%: $(TEST_DIR)/%
	@echo "Copying $< to $@"
	@mkdir -p $(dir $@)
	@cp $< $@

# Run tests
test: build build-cnf test-setup 
	@echo "Running tests..."
	@cd $(BUILD_TEST) && $(QEXE) unit/run.q -q

# Clean build and temporary files
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

# Prune obsolete files
prune:
	@echo "Removing obsolete files..."
	@if [ -d $(BUILD_SRC) ]; then \
		find $(BUILD_SRC) -type f -exec bash -c 'SRC="$${1#$(BUILD_SRC)/}"; [ ! -f "$(SRC_DIR)/$$SRC" ] && rm -f "$$1"' _ {} \;; \
	fi
	@if [ -d $(BUILD_TEST) ]; then \
		find $(BUILD_TEST) -type f -exec bash -c 'SRC="$${1#$(BUILD_TEST)/}"; [ ! -f "$(TEST_DIR)/$$SRC" ] && rm -f "$$1"' _ {} \;; \
	fi
	@if [ -d $(BUILD_CNF) ]; then \
		find $(BUILD_CNF) -type f -exec bash -c 'SRC="$${1#$(BUILD_CNF)/}"; [ ! -f "$(CNF_DIR)/$$SRC" ] && rm -f "$$1"' _ {} \;; \
	fi

.PHONY: all setup build build-cnf test-setup test clean prune
