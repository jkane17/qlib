# QLib

_QLib_ is a modular library for the [Q programming language](https://code.kx.com/q/). It consists of:

- **Q modules** (`src/q`): libraries written in Q, such as a command-line argument parser and a unit testing framework. See [Modules](#modules).
- **A C interface** (`src/c/cdk`): a set of C headers for creating, inspecting, and managing Q objects from C or C++, wrapping the kdb+ C API (`k.h`). It is built into a shared library, `libcdk.so`. See the [C interface documentation](doc/c/README.md).

## Quick Start

```bash
./build.sh -i     # build and install to $QHOME/mod/qlib (or ~/.kx/mod/qlib)
```

Then, in Q:

```q
clap:use`qlib.clap
```

## Requirements

| Requirement                               | Needed for                                  |
| ----------------------------------------- | ------------------------------------------- |
| Bash                                      | Running `build.sh`                          |
| A C compiler with C23 support (`-std=c2x`), GCC or Clang | Building `libcdk.so`             |
| A C++ compiler with C++20 support         | C++ tests (`--ctest`)                       |
| `q` on the `PATH`                         | Q tests (`--qtest`)                         |

The C interface is tested with GCC 13 and Clang 18. The compilers can be changed with the `CC` and `CXX` environment variables (see [Environment Variables](#environment-variables)).

`build.sh` supports Linux, macOS, and Windows (MinGW, MSYS, or Cygwin) on x86-64 and ARM64. However, the C and documentation tests link against the copy of KX's `c.o` in `src/c/obj`, which is for 64-bit Linux on x86-64 only. To run them on another platform, replace it with the `c.o` for that platform from KX.

## Building

```bash
./build.sh
```

This clears the `build` directory, then builds the library into `build/qlib`:

| File                | Description                                      |
| ------------------- | ------------------------------------------------ |
| `*.q`               | The Q modules, copied from `src/q`               |
| `libcdk.so`         | The C interface, compiled from `src/c/cdk/*.c`   |

By default, `libcdk.so` is a debug build (`-g -O0`), with assertions enabled. For an optimised build with assertions removed (`-O3 -DNDEBUG`), add `--release`:

```bash
./build.sh --release
```

`--release` is ignored when running tests, as the tests rely on assertions.

To remove the contents of the `build` directory without building, run:

```bash
./build.sh --clean
```

## Installing

```bash
./build.sh -i
```

This builds the library and copies the contents of `build/qlib` to the install directory. By default, the install directory is:

- `$QHOME/mod/qlib`, if `QHOME` is set, or
- `~/.kx/mod/qlib` otherwise.

These are within Q's module search path, so the modules can then be loaded with `use`, for example ``use`qlib.clap``.

To install somewhere else, use `-d`:

```bash
./build.sh -i -d /path/to/qlib
```

> **Warning:** Installing **deletes the existing contents** of the install directory before copying the new files. Do not install into a directory that contains anything else.

Installation can be combined with tests. For example, to install only if every test passes:

```bash
./build.sh -t -i
```

If a C or documentation test fails, installation is skipped. If a Q test fails, `build.sh` stops immediately, so nothing after it runs.

## Testing

Tests always use a debug build, so that assertions are enabled.

| Option      | Tests run                                                              |
| ----------- | ---------------------------------------------------------------------- |
| `-t`        | All of the tests below (except `--itest`)                              |
| `--qtest`   | Q unit tests                                                           |
| `--ctest`   | C and C++ unit tests, and header checks                                |
| `--doctest` | The C examples in the documentation                                    |
| `--itest`   | Starts a Q process with the Q tests loaded, for interactive use        |

The options can be combined, for example `./build.sh --ctest --doctest`. When several are given, the Q tests run first.

### Q Tests (`--qtest`)

Runs the Q unit tests in `test/q` with the [`unit`](doc/unit.md) module, against the modules in `build`. A summary and any failures are printed. If any test fails, `build.sh` exits with a non-zero status straight away.

`--itest` instead starts a Q process listening on port `5000`, with the tests registered but not run, so that they can be run and debugged interactively.

### C and C++ Tests (`--ctest`)

1. **Header checks**: every header in `src/c/cdk` is compiled on its own, as both C23 and C++20, to check that it includes everything it needs. The checks use `-Wcast-qual`, so that the headers also work in projects that enable it.
2. **Unit tests**: every `test/c/test_*.c` and `test/c/test_*.cpp` file is built with [Unity](https://github.com/ThrowTheSwitch/Unity) and run. The C++ tests check that the headers work from C++, and that the functions are declared with C linkage.

### Documentation Tests (`--doctest`)

Every C example in `doc/c/*.md` that has a `main` function and is followed by an `Output:` block is extracted, built, and run. The test passes if the program exits with status `0` and prints exactly the documented output (ignoring trailing whitespace). This keeps the examples in the documentation compiling and correct.

For an example to be tested, it must be written like this:

````markdown
```c
#include <stdio.h>
#include "q.h"

int main() {
    ...
}
```

Output:

```
...
```
````

### Test Builds and Results

The C, C++, and documentation tests are built with `-Wall -Wextra -Werror`, and against a separate copy of `libcdk.so` in `build/test`, which is not installed. Where it is available (not on MinGW), they are also built with the undefined behaviour sanitizer, so any undefined behaviour fails the test.

When the tests finish, `build.sh` prints every result, followed by a summary:

```
Tests 251 | Passed 251 | Failed 0
```

A test that fails to build, crashes, or fails is listed with a message. Details are written to:

| File                                | Contents                                           |
| ----------------------------------- | -------------------------------------------------- |
| `build/test/test_fail_output.log`   | The full output of a C or C++ test that crashed    |
| `build/doctest/<doc>_<line>.build.log` | The compiler output for a documentation example that failed to build |
| `build/doctest/<doc>_<line>.out`    | The output of a documentation example              |
| `build/doctest/<doc>_<line>.diff`   | The difference from the documented output          |

`<doc>_<line>` identifies the example by the documentation file and the line on which the example starts.

`build.sh` exits with a non-zero status if any test fails, so it can be used in CI.

## Reference

### Options

| Option              | Description                                                           |
| ------------------- | --------------------------------------------------------------------- |
| `-h`, `--help`      | Show the usage message and exit                                       |
| `-i`, `--install`   | Install the library after building (and testing, if requested)        |
| `-d`, `--dir <dir>` | Install to `<dir>` instead of the default install directory          |
| `-t`, `--test`      | Run all tests (Q, C and C++, and documentation)                       |
| `--qtest`           | Run the Q unit tests                                                  |
| `--ctest`           | Run the C and C++ unit tests and header checks                        |
| `--doctest`         | Build and run the documentation examples in `doc/c`                   |
| `--itest`           | Start a Q process on port `5000` for interactive testing              |
| `--release`         | Build an optimised `libcdk.so` without assertions (ignored with tests) |
| `--clean`           | Remove the contents of the `build` directory and exit                 |

### Environment Variables

| Variable | Description                         | Default |
| -------- | ----------------------------------- | ------- |
| `CC`     | C compiler                          | `gcc`   |
| `CXX`    | C++ compiler                        | `g++`   |
| `QHOME`  | Used for the default install directory (`$QHOME/mod/qlib`) | Not set |

For example, to build and test with Clang:

```bash
CC=clang CXX=clang++ ./build.sh -t
```

## Modules

| Module | Description                               | Documentation          | Dependencies |
| ------ | ----------------------------------------- | ---------------------- | ------------ |
| `clap` | Command-Line Argument Parser              | [clap.md](doc/clap.md) |              |
| `dbm`  | Database Maintenance                      | [dbm.md](doc/dbm.md)   | `fs`         |
| `fs`   | File System Operations (Work in Progress) | [fs.md](doc/fs.md)     |              |
| `unit` | Unit Testing Framework (Work in Progress) | [unit.md](doc/unit.md) | `fs`         |

The C interface is documented separately, in [doc/c](doc/c/README.md).

## Repository Layout

| Directory     | Contents                                                        |
| ------------- | --------------------------------------------------------------- |
| `src/q`       | Q modules                                                       |
| `src/c/cdk`   | C interface headers and sources                                 |
| `src/c/obj`   | KX's `c.o`, used to link the C tests                            |
| `test/q`      | Q unit tests                                                    |
| `test/c`      | C and C++ unit tests, and the Unity test framework              |
| `doc`         | Module documentation (`doc/c` for the C interface)              |
| `build`       | Build output (created by `build.sh`)                            |

## Contributing

If you'd like to contribute a module or fix an issue, please open a pull request or start a discussion in the issues tab. Please run `./build.sh -t` before submitting a pull request.
