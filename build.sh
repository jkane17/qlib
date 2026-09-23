#!/bin/bash

usage() {
    echo "Usage: $(basename $0) [options]"
    echo "  -d|--dir <directory> Set custom install directory"
    echo "  -h|--help            Show this help message and exit"
    echo "  -i|--install         Install"
    echo "  -t|--test            Execute all unit tests"
    echo "  --clean              Remove build directory contents"
    echo "  --itest              Interactive test mode"
    echo "  --qtest              Execute Q unit tests"
    echo "  --ctest              Execute C unit tests"
    echo "  --release            Release build"
}

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BUILD_QLIB_DIR="${BUILD_DIR}/qlib"
SRC_DIR="${PROJECT_DIR}/src"
TEST_DIR="${PROJECT_DIR}/test"

Q_SRC_DIR="${SRC_DIR}/q"
Q_TEST_DIR="${TEST_DIR}/q"

C_SRC_DIR="${SRC_DIR}/c"
C_TEST_DIR="${TEST_DIR}/c"
C_CDK_DIR="${C_SRC_DIR}/cdk"
C_LIB_DIR="${C_SRC_DIR}/lib"
C_UNITY_DIR="${C_TEST_DIR}/unity"
C_OBJ_FILE="${C_SRC_DIR}/obj/c.o"

if [ -z "${QHOME}" ]; then
    INSTALL_DIR="${HOME}/.kx/mod/qlib/"
else
    INSTALL_DIR="${QHOME}/mod/qlib/"
fi

CC=/usr/bin/gcc
STD=c2x
CFLAGS=("-std=${STD}" "-I${C_SRC_DIR}" "-I${C_CDK_DIR}" -Wall -Wextra -Werror)
LFLAGS=(-lm)
CDK_SO="${BUILD_QLIB_DIR}/libcdk.so"

INTERACTIVE_PORT=5000

CLEAN=false
INSTALL=false
RELEASE=false
TEST=false
ITEST=false
QTEST=false
CTEST=false

GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -d|--dir)
            if [ -z "$2" ]; then
                echo "Error: -d requires a directory argument"
                usage
                exit 1
            fi
            INSTALL_DIR="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -t|--test)
            TEST=true
            shift
            ;;
        --itest)
            ITEST=true
            shift
            ;;
        --qtest)
            QTEST=true
            shift
            ;;
        --ctest)
            CTEST=true
            shift
            ;;
       --clean)
            CLEAN=true
            shift
            ;;
        --release)
            RELEASE=true
            shift
            ;;
        --)
            shift
            break
            ;;
        -*)
            echo "Unknown option: $1" >&2
            usage
            exit 1
            ;;
        *)
            break
            ;;
    esac
done

create_dir() {
    mkdir -p "$1" || { echo "Error: Failed to create directory $1"; exit 1; }
}

clean_dir() {
    rm -rf "$1"/* || { echo "Error: Failed to clean directory $1"; exit 1; }
}

copy_files() {
    cp "$@" || { echo "Error: Failed to copy files"; exit 1; }
}

compile_cdk() {
    "${CC}" "${CFLAGS[@]}" -fPIC -shared -o "${CDK_SO}" "${C_CDK_DIR}"/*.c "${LFLAGS[@]}" || {
        echo "Compilation failed for ${CDK_SO}" >&2
        exit 1
    }
}

# $1 : Library name
compile_library() {
    "${CC}" "${CFLAGS[@]}" -fPIC -shared \
        -o "${BUILD_QLIB_DIR}/${1}.${PLATFORM}.so" \
        "${C_LIB_DIR}/${1}.c" "${CDK_SO}" \
        "${LFLAGS[@]}" || 
        {
            echo "Compilation failed for ${1}" >&2
            exit 1
        }
}

compile_test() {
    local src_file="$1"
    local exe_file="$2"

    "${CC}" "${CFLAGS[@]}" \
        "-I${C_UNITY_DIR}" \
        "${C_UNITY_DIR}"/*.c \
        "${src_file}" \
        "${CDK_SO}" \
        "${C_OBJ_FILE}" \
        "${LDFLAGS[@]}" \
        "-DUNITY_INCLUDE_CONFIG_H" \
        -o "${exe_file}" || {
            echo "Compilation failed for ${src_file}" >&2
            exit 1
        }
}

run_q_tests() {
    CMD=(q "${Q_TEST_DIR}/run.q" -modulePath "${BUILD_DIR}")

    if ${ITEST}; then
        CMD+=(-itest -p "${INTERACTIVE_PORT}")
    else
        echo "Running Q unit tests.."
    fi

    "${CMD[@]}" || {
        echo "Failed to run Q unit tests" >&2
        exit 1
    }
}

print_test_line() {
    local line="$1"

    IFS=':' read -r path lineno test result message <<< "$line"

    file=${path##*/}

    if [[ $result == "PASS" ]]; then
        result_col="${GREEN}PASS${RESET}"
    else
        result_col="${RED}FAIL${RESET}"
    fi

    printf "%-*s:%-*s  %-*s  %b %s\n" \
        "$file_w" "$file" \
        "$line_w" "$lineno" \
        "$test_w" "$test" \
        "$result_col" "$message"
}

banner_line() {
    printf '%*s\n' "$1" '' | tr ' ' '='
}

banner() {
    echo
    banner_line $2
    echo -e "$1"
    banner_line $2
}

run_c_tests() {
    build_test_dir="${BUILD_DIR}/test"
    create_dir "${build_test_dir}"

    echo "Building C unit tests.."
    for src_file in "${C_TEST_DIR}"/test_*.c; do
        [ -e "${src_file}" ] || continue

        exe_name="${build_test_dir}/$(basename "${src_file%.c}")"
        compile_test "$src_file" "$exe_name"
    done

    echo "Running C unit tests.."

    export LD_LIBRARY_PATH="${build_test_dir}:${LD_LIBRARY_PATH}"

    pass_results=()
    fail_results=()
    
    for file in "${build_test_dir}"/test_*; do
        [ -e "${file}" ] || continue

        echo "Running ${file}"
        
        output="$("${file}" 2>&1)"
        status=$?

        while IFS= read -r line; do
            case "$line" in
                *:PASS) pass_results+=("$line") ;;
                *:FAIL*) fail_results+=("$line") ;;
            esac
        done <<< "$output"

        # If the test crashed, add a generic failure line for visibility
        if (( status > 1 )); then
            fail_results+=("${file}:FAIL (exit code ${status})")
            # Optionally also save full output to a log
            echo "$output" >> "${build_test_dir}/test_fail_output.log"
        fi
    done

    total=$(( ${#pass_results[@]} + ${#fail_results[@]} ))

    file_w=0
    line_w=0
    test_w=0

    for line in "${pass_results[@]}" "${fail_results[@]}"; do
        IFS=':' read -r path lineno test result message <<< "$line"

        file=${path##*/}

        (( ${#file}  > file_w )) && file_w=${#file}
        (( ${#lineno} > line_w )) && line_w=${#lineno}
        (( ${#test}  > test_w )) && test_w=${#test}
    done

    result_w=4
    table_w=$((file_w + 1 + line_w + 2 + test_w + 2 + result_w + 1 + 20))

    banner "${GREEN}PASSED${RESET}" $table_w
    for line in "${pass_results[@]}"; do
        print_test_line "$line"
    done

    if (( ${#fail_results[@]} > 0 )); then
        banner "${RED}FAILED${RESET}" $table_w
        for line in "${fail_results[@]}"; do
            print_test_line "$line"
        done

        echo
        echo -e "Tests ${total} | ${GREEN}Passed ${#pass_results[@]}${RESET} | ${RED}Failed ${#fail_results[@]}${RESET}"
    else
        echo
        echo -e "${GREEN}Tests ${total} | Passed ${#pass_results[@]} | Failed 0${RESET}"
    fi
}

if ${CLEAN}; then
    echo "Cleaning ${BUILD_DIR} ..."
    clean_dir "${BUILD_DIR}"
    exit 0
fi

if ${RELEASE} && !(${TEST} || ${ITEST}); then
    CFLAGS+=(-O3 -DNDEBUG)
else
    CFLAGS+=(-g -O0)
fi

OS=$(uname -s)
ARCH=$(uname -m)
BITS=$(getconf LONG_BIT)

# OS identifier
case "${OS}" in
    Linux)  OS_ID="l" ;;
    Darwin) OS_ID="m" ;;
    MINGW*|MSYS*|CYGWIN*) OS_ID="w" ;;
    *) echo "Unsupported OS: ${OS}"; exit 1 ;;
esac

# Architecture identifier
case "${ARCH}" in
    x86_64|i386|i686) ARCH_ID="i" ;;
    aarch64|arm64)    ARCH_ID="a" ;;
    *) echo "Unsupported arch: ${ARCH}"; exit 1 ;;
esac

PLATFORM="${OS_ID}${ARCH_ID}${BITS}"

echo "Building ${BUILD_DIR}/"
create_dir "${BUILD_DIR}"
clean_dir "${BUILD_DIR}"
create_dir "${BUILD_QLIB_DIR}"
copy_files "${Q_SRC_DIR}"/*.q "${BUILD_QLIB_DIR}"
compile_cdk
# compile_library "fstr"
echo "Build complete"

if ${TEST} || ${ITEST} || ${QTEST}; then
    echo
    run_q_tests
fi

if ${TEST} || ${CTEST}; then
    echo
    run_c_tests
fi

if ${INSTALL}; then
    echo
    echo "Installing library to ${INSTALL_DIR}"

    create_dir "${INSTALL_DIR}"
    clean_dir "${INSTALL_DIR}"
    copy_files "${BUILD_QLIB_DIR}"/* "${INSTALL_DIR}"

    echo "Installation complete."
fi
