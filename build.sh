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
CXX=/usr/bin/g++
CXXSTD=c++20
CXXFLAGS=("-std=${CXXSTD}" "-I${C_SRC_DIR}" "-I${C_CDK_DIR}" -Wall -Wextra -Werror)
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

    if [[ "${src_file}" == *.cpp ]]; then
        compile_cpp_test "${src_file}" "${exe_file}"
        return
    fi

    "${CC}" "${CFLAGS[@]}" \
        "-I${C_UNITY_DIR}" \
        "${C_UNITY_DIR}"/*.c \
        "${src_file}" \
        "-L${BUILD_QLIB_DIR}" -lcdk \
        "-Wl,-rpath,\$ORIGIN/../qlib" \
        "${C_OBJ_FILE}" \
        "${LFLAGS[@]}" \
        "-DUNITY_INCLUDE_CONFIG_H" \
        -o "${exe_file}" || {
            echo "Compilation failed for ${src_file}" >&2
            return 1
        }
}

# Compile a C++ test: Unity is compiled as C, the test as C++, and both are linked against the
# C-built libcdk.so (which checks that the C functions are declared with C linkage).
compile_cpp_test() {
    local src_file="$1"
    local exe_file="$2"
    local unity_obj="${build_test_dir}/unity.o"

    if [ ! -e "${unity_obj}" ]; then
        "${CC}" "${CFLAGS[@]}" "-DUNITY_INCLUDE_CONFIG_H" \
            -c "${C_UNITY_DIR}/unity.c" -o "${unity_obj}" || {
                echo "Compilation failed for ${C_UNITY_DIR}/unity.c" >&2
                exit 1
            }
    fi

    "${CXX}" "${CXXFLAGS[@]}" \
        "-I${C_UNITY_DIR}" \
        "-DUNITY_INCLUDE_CONFIG_H" \
        "${src_file}" \
        "${unity_obj}" \
        "-L${BUILD_QLIB_DIR}" -lcdk \
        "-Wl,-rpath,\$ORIGIN/../qlib" \
        "${C_OBJ_FILE}" \
        "${LFLAGS[@]}" \
        -o "${exe_file}" || {
            echo "Compilation failed for ${src_file}" >&2
            return 1
        }
}

# Check that every cdk header compiles on its own (includes everything it needs) as both C and
# C++. Results are added to the pass/fail results of the C tests.
check_headers() {
    local header name error

    for header in "${C_CDK_DIR}"/*.h; do
        name="$(basename "${header}")"

        error="$(printf '#include <%s>\n' "${name}" |
            "${CC}" "${CFLAGS[@]}" -fsyntax-only -x c - 2>&1)"
        if [ -z "${error}" ]; then
            pass_results+=("${name}:0:compiles standalone as ${STD}:PASS")
        else
            fail_results+=("${name}:0:compiles standalone as ${STD}:FAIL:$(grep -m1 'error' <<< "${error}")")
        fi

        error="$(printf '#include <%s>\n' "${name}" |
            "${CXX}" "${CXXFLAGS[@]}" -fsyntax-only -x c++ - 2>&1)"
        if [ -z "${error}" ]; then
            pass_results+=("${name}:0:compiles standalone as ${CXXSTD}:PASS")
        else
            fail_results+=("${name}:0:compiles standalone as ${CXXSTD}:FAIL:$(grep -m1 'error' <<< "${error}")")
        fi
    done
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

    pass_results=()
    fail_results=()

    echo "Checking headers compile standalone as C and C++.."
    check_headers

    # A test that fails to compile is reported as a failure, and the remaining tests still run
    echo "Building C and C++ unit tests.."
    for src_file in "${C_TEST_DIR}"/test_*.c "${C_TEST_DIR}"/test_*.cpp; do
        [ -e "${src_file}" ] || continue

        exe_name="${build_test_dir}/$(basename "${src_file%.*}")"
        compile_test "$src_file" "$exe_name" ||
            fail_results+=("${src_file}:0:(build):FAIL:compilation failed, see output above")
    done

    echo "Running C and C++ unit tests.."
    
    for file in "${build_test_dir}"/test_*; do
        [ -f "${file}" ] && [ -x "${file}" ] || continue

        echo "Running ${file}"
        
        output="$("${file}" 2>&1)"
        status=$?

        file_fails=0
        while IFS= read -r line; do
            case "$line" in
                *:PASS) pass_results+=("$line") ;;
                *:FAIL*) fail_results+=("$line"); file_fails=$((file_fails + 1)) ;;
            esac
        done <<< "$output"

        # Unity exits with the number of failed tests, so a non-zero status is only unexpected if it
        # was caused by a signal (>= 128, e.g. a segfault or failed assert) or no failures were
        # reported. Add a generic failure line for visibility and save the full output to a log.
        if (( status >= 128 || (status != 0 && file_fails == 0) )); then
            fail_results+=("${file}:0:(crashed):FAIL:exit code ${status}, see test_fail_output.log")
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
        return 1
    fi

    echo
    echo -e "${GREEN}Tests ${total} | Passed ${#pass_results[@]} | Failed 0${RESET}"
}

if ${CLEAN}; then
    echo "Cleaning ${BUILD_DIR} ..."
    clean_dir "${BUILD_DIR}"
    exit 0
fi

if ${RELEASE} && !(${TEST} || ${ITEST}); then
    CFLAGS+=(-O3 -DNDEBUG)
    CXXFLAGS+=(-O3 -DNDEBUG)
else
    CFLAGS+=(-g -O0)
    CXXFLAGS+=(-g -O0)
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

TEST_STATUS=0

if ${TEST} || ${CTEST}; then
    echo
    run_c_tests || TEST_STATUS=1
fi

if ${INSTALL} && (( TEST_STATUS != 0 )); then
    echo
    echo "Tests failed, skipping installation." >&2
elif ${INSTALL}; then
    echo
    echo "Installing library to ${INSTALL_DIR}"

    create_dir "${INSTALL_DIR}"
    clean_dir "${INSTALL_DIR}"
    copy_files "${BUILD_QLIB_DIR}"/* "${INSTALL_DIR}"

    echo "Installation complete."
fi

exit ${TEST_STATUS}
