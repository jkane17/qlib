#!/bin/bash

set -uo pipefail

usage() {
    echo "Usage: $(basename "$0") [options]"
    echo "  -d|--dir <directory> Set custom install directory"
    echo "  -h|--help            Show this help message and exit"
    echo "  -i|--install         Install"
    echo "  -t|--test            Execute all tests (Q, C/C++ and documentation examples)"
    echo "  --clean              Remove build directory contents"
    echo "  --itest              Interactive test mode"
    echo "  --qtest              Execute Q unit tests"
    echo "  --ctest              Execute C and C++ unit tests"
    echo "  --doctest            Build and run the examples in doc/c and check their output"
    echo "  --release            Release build (ignored when running tests)"
    echo
    echo "Environment:"
    echo "  CC                   C compiler (default: gcc)"
    echo "  CXX                  C++ compiler (default: g++)"
    echo "  QCLIB                KX's C API library to link the C tests with (required by --ctest and"
    echo "                       --doctest), e.g. \"\$HOME/kdb/l64/c.o\" or \"\$HOME/kdb/l64/e.o -lssl -lcrypto\""
}

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BUILD_QLIB_DIR="${BUILD_DIR}/qlib"
BUILD_INCLUDE_DIR="${BUILD_QLIB_DIR}/include"
BUILD_TEST_DIR="${BUILD_DIR}/test"
BUILD_DOCTEST_DIR="${BUILD_DIR}/doctest"
SRC_DIR="${PROJECT_DIR}/src"
TEST_DIR="${PROJECT_DIR}/test"
DOC_DIR="${PROJECT_DIR}/doc"

Q_SRC_DIR="${SRC_DIR}/q"
Q_TEST_DIR="${TEST_DIR}/q"

C_SRC_DIR="${SRC_DIR}/c"
C_TEST_DIR="${TEST_DIR}/c"
C_DOC_DIR="${DOC_DIR}/c"
C_CDK_DIR="${C_SRC_DIR}/cdk"
C_LIB_DIR="${C_SRC_DIR}/lib"
C_UNITY_DIR="${C_TEST_DIR}/unity"
# KX's C API library (c.o, e.o, c.lib, ...) is not distributed with QLib, so it must be provided to
# link the C tests. QCLIB holds the path to the library file, followed by any other linker arguments
# it needs (e.g. -lssl -lcrypto for e.o), separated by spaces.
read -r -a QCLIB_ARGS <<< "${QCLIB:-}"

if [ -z "${QHOME:-}" ]; then
    INSTALL_DIR="${HOME}/.kx/mod/qlib/"
else
    INSTALL_DIR="${QHOME}/mod/qlib/"
fi

CC="${CC:-gcc}"
STD=c2x
CFLAGS=("-std=${STD}" "-I${C_SRC_DIR}" "-I${C_CDK_DIR}" -Wall -Wextra -Werror)
CXX="${CXX:-g++}"
CXXSTD=c++20
CXXFLAGS=("-std=${CXXSTD}" "-I${C_SRC_DIR}" "-I${C_CDK_DIR}" -Wall -Wextra -Werror)
LFLAGS=(-lm)
CDK_SO="${BUILD_QLIB_DIR}/libcdk.so"

# Tests are built with the undefined behaviour sanitizer where it is available (set below), against
# a sanitized copy of libcdk.so in the test build directory
SANITIZE_FLAGS=()
TEST_CDK_SO="${BUILD_TEST_DIR}/libcdk.so"

INTERACTIVE_PORT=5000

CLEAN=false
INSTALL=false
RELEASE=false
TEST=false
ITEST=false
QTEST=false
CTEST=false
DOCTEST=false

GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"

# Test results, each of the form <file>:<line>:<test>:<PASS|FAIL>[:<message>]
pass_results=()
fail_results=()

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -d|--dir)
            if [ -z "${2:-}" ]; then
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
        --doctest)
            DOCTEST=true
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

# Build the sanitized copy of libcdk.so used by the tests (not installed)
compile_test_cdk() {
    create_dir "${BUILD_TEST_DIR}"
    "${CC}" "${CFLAGS[@]}" "${SANITIZE_FLAGS[@]}" -fPIC -shared -o "${TEST_CDK_SO}" \
        "${C_CDK_DIR}"/*.c "${LFLAGS[@]}" || {
            echo "Compilation failed for ${TEST_CDK_SO}" >&2
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

# Set test_link to the link flags for an executable that uses the test copy of libcdk.so
# $1 : Path from the executable's directory to the test build directory
test_link_flags() {
    test_link=("-L${BUILD_TEST_DIR}" -lcdk "-Wl,-rpath,\$ORIGIN/$1" "${QCLIB_ARGS[@]}" "${LFLAGS[@]}")
}

compile_test() {
    local src_file="$1"
    local exe_file="$2"

    if [[ "${src_file}" == *.cpp ]]; then
        compile_cpp_test "${src_file}" "${exe_file}"
        return
    fi

    test_link_flags "."
    "${CC}" "${CFLAGS[@]}" "${SANITIZE_FLAGS[@]}" \
        "-I${C_UNITY_DIR}" \
        "${C_UNITY_DIR}"/*.c \
        "${src_file}" \
        "${test_link[@]}" \
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
    local unity_obj="${BUILD_TEST_DIR}/unity.o"

    if [ ! -e "${unity_obj}" ]; then
        "${CC}" "${CFLAGS[@]}" "${SANITIZE_FLAGS[@]}" "-DUNITY_INCLUDE_CONFIG_H" \
            -c "${C_UNITY_DIR}/unity.c" -o "${unity_obj}" || {
                echo "Compilation failed for ${C_UNITY_DIR}/unity.c" >&2
                exit 1
            }
    fi

    test_link_flags "."
    "${CXX}" "${CXXFLAGS[@]}" "${SANITIZE_FLAGS[@]}" \
        "-I${C_UNITY_DIR}" \
        "-DUNITY_INCLUDE_CONFIG_H" \
        "${src_file}" \
        "${unity_obj}" \
        "${test_link[@]}" \
        -o "${exe_file}" || {
            echo "Compilation failed for ${src_file}" >&2
            return 1
        }
}

# Check that every cdk header compiles on its own (includes everything it needs) as both C and
# C++, with -Wcast-qual so that the headers work in projects that enable it. Results are added to
# the pass/fail results.
check_headers() {
    local header name error

    for header in "${C_CDK_DIR}"/*.h; do
        name="$(basename "${header}")"

        error="$(printf '#include <%s>\n' "${name}" |
            "${CC}" "${CFLAGS[@]}" -Wcast-qual -fsyntax-only -x c - 2>&1)"
        if [ -z "${error}" ]; then
            pass_results+=("${name}:0:compiles standalone as ${STD}:PASS")
        else
            fail_results+=("${name}:0:compiles standalone as ${STD}:FAIL:$(grep -m1 'error' <<< "${error}")")
        fi

        error="$(printf '#include <%s>\n' "${name}" |
            "${CXX}" "${CXXFLAGS[@]}" -Wcast-qual -fsyntax-only -x c++ - 2>&1)"
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
    banner_line "$2"
    echo -e "$1"
    banner_line "$2"
}

run_c_tests() {
    echo "Checking headers compile standalone as C and C++.."
    check_headers

    # A test that fails to compile is reported as a failure, and the remaining tests still run
    echo "Building C and C++ unit tests.."
    for src_file in "${C_TEST_DIR}"/test_*.c "${C_TEST_DIR}"/test_*.cpp; do
        [ -e "${src_file}" ] || continue

        exe_name="${BUILD_TEST_DIR}/$(basename "${src_file%.*}")"
        compile_test "$src_file" "$exe_name" ||
            fail_results+=("${src_file}:0:(build):FAIL:compilation failed, see output above")
    done

    echo "Running C and C++ unit tests.."

    for file in "${BUILD_TEST_DIR}"/test_*; do
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
            echo "$output" >> "${BUILD_TEST_DIR}/test_fail_output.log"
        fi
    done
}

# Extract the C examples from a markdown file that have a main function and are followed by an
# "Output:" block. Each example is written to <out_dir>/<name>_<line>.c and its expected output to
# <out_dir>/<name>_<line>.exp, where <line> is the line of the example in the markdown file.
# $1 : Markdown file
# $2 : Output directory
extract_doc_examples() {
    local name
    name="$(basename "$1" .md)"

    awk -v out="$2" -v name="${name}" '
        function reset() { pending = 0; saw_output = 0 }
        in_code && /^```[[:space:]]*$/ { in_code = 0; pending = (code ~ /int main/); saw_output = 0; next }
        in_code { code = code $0 "\n"; next }
        in_output && /^```[[:space:]]*$/ {
            base = out "/" name "_" start
            printf "%s", code > (base ".c"); close(base ".c")
            printf "%s", expected > (base ".exp"); close(base ".exp")
            in_output = 0; reset(); next
        }
        in_output { expected = expected $0 "\n"; next }
        /^```c[[:space:]]*$/ { in_code = 1; start = NR; code = ""; reset(); next }
        pending && !saw_output && /^Output:/ { saw_output = 1; next }
        pending && saw_output && /^```/ { in_output = 1; expected = ""; next }
        pending && NF > 0 { reset() }
    ' "$1"
}

run_doc_tests() {
    local md example base name status result_id

    create_dir "${BUILD_DOCTEST_DIR}"

    echo "Extracting documentation examples.."
    for md in "${C_DOC_DIR}"/*.md; do
        extract_doc_examples "${md}" "${BUILD_DOCTEST_DIR}"
    done

    echo "Building and running documentation examples.."
    test_link_flags "../test"
    for example in "${BUILD_DOCTEST_DIR}"/*.c; do
        [ -e "${example}" ] || continue

        base="${example%.c}"
        name="$(basename "${base}")"
        # <doc>_<line> -> <doc>.md:<line>
        result_id="${name%_*}.md:${name##*_}:documentation example"

        if ! "${CC}" "${CFLAGS[@]}" "${SANITIZE_FLAGS[@]}" "${example}" "${test_link[@]}" \
            -o "${base}" 2> "${base}.build.log"; then
            fail_results+=("${result_id}:FAIL:compilation failed, see ${base}.build.log")
            continue
        fi

        "${base}" > "${base}.out" 2>&1
        status=$?
        if (( status != 0 )); then
            fail_results+=("${result_id}:FAIL:exit code ${status}, see ${base}.out")
        elif ! diff <(sed 's/[[:space:]]*$//' "${base}.exp") <(sed 's/[[:space:]]*$//' "${base}.out") \
            > "${base}.diff"; then
            fail_results+=("${result_id}:FAIL:output differs from documented output, see ${base}.diff")
        else
            pass_results+=("${result_id}:PASS")
        fi
    done
}

# Print the collected results. Returns non-zero if any test failed.
print_results() {
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

RUN_TESTS=false
if ${TEST} || ${ITEST} || ${QTEST} || ${CTEST} || ${DOCTEST}; then
    RUN_TESTS=true
fi

# Tests always use a debug build (so that assertions are enabled)
if ${RELEASE} && ${RUN_TESTS}; then
    echo "Warning: --release is ignored when running tests" >&2
fi

if ${RELEASE} && ! ${RUN_TESTS}; then
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

# The undefined behaviour sanitizer is not available with MinGW. Any undefined behaviour aborts the
# test, so it is reported as a failure.
if [[ "${OS_ID}" != "w" ]]; then
    SANITIZE_FLAGS=(-fsanitize=undefined -fno-sanitize-recover=undefined)
fi

echo "Building ${BUILD_DIR}/"
create_dir "${BUILD_DIR}"
clean_dir "${BUILD_DIR}"
create_dir "${BUILD_QLIB_DIR}"
copy_files "${Q_SRC_DIR}"/*.q "${BUILD_QLIB_DIR}"
create_dir "${BUILD_INCLUDE_DIR}"
copy_files "${C_CDK_DIR}"/*.h "${BUILD_INCLUDE_DIR}"
compile_cdk
# compile_library "fstr"
echo "Build complete"

if ${TEST} || ${ITEST} || ${QTEST}; then
    echo
    run_q_tests
fi

TEST_STATUS=0
RUN_C_TESTS=false
RUN_DOC_TESTS=false
if ${TEST} || ${CTEST}; then
    RUN_C_TESTS=true
fi
if ${TEST} || ${DOCTEST}; then
    RUN_DOC_TESTS=true
fi

if ${RUN_C_TESTS} || ${RUN_DOC_TESTS}; then
    if (( ${#QCLIB_ARGS[@]} == 0 )); then
        echo "Error: QCLIB is not set" >&2
        echo "The C and documentation tests link against KX's C API library. Download the files for" >&2
        echo "your platform from https://github.com/KxSystems/kdb, then set QCLIB to the library to" >&2
        echo "link, e.g. QCLIB=\"\$HOME/kdb/l64/c.o\" ./build.sh --ctest" >&2
        exit 1
    fi
    if [ ! -f "${QCLIB_ARGS[0]}" ]; then
        echo "Error: KX's C API library was not found at ${QCLIB_ARGS[0]} (from QCLIB)" >&2
        exit 1
    fi
    echo
    compile_test_cdk
fi

if ${RUN_C_TESTS}; then
    run_c_tests
fi

if ${RUN_DOC_TESTS}; then
    run_doc_tests
fi

if ${RUN_C_TESTS} || ${RUN_DOC_TESTS}; then
    print_results || TEST_STATUS=1
fi

if ${INSTALL} && (( TEST_STATUS != 0 )); then
    echo
    echo "Tests failed, skipping installation." >&2
elif ${INSTALL}; then
    echo
    echo "Installing library to ${INSTALL_DIR}"

    create_dir "${INSTALL_DIR}"
    clean_dir "${INSTALL_DIR}"
    copy_files -r "${BUILD_QLIB_DIR}"/* "${INSTALL_DIR}"

    echo "Installation complete."
fi

exit ${TEST_STATUS}
