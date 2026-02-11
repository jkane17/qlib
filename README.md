# QLib

_QLib_ is a modular library for the [Q programming language](https://code.kx.com/q/).

It is recommended to copy all source files and place them under the `qlib` subdirectory within your module search path, e.g., `/home/user/.kx/mod/qlib/.`.

## Modules

| Module | Description                               | Documentation          | Dependencies |
| ------ | ----------------------------------------- | ---------------------- | ------------ |
| `clap` | Command-Line Argument Parser              | [clap.md](doc/clap.md) |              |
| `dbm`  | Database Maintenance                      | [dbm.md](doc/dbm.md)   |              |
| `fs`   | File System Operations (Work in Progress) | [fs.md](doc/fs.md)     |              |
| `unit` | Unit Testing Framework (Work in Progress) | [unit.md](doc/unit.md) | `fs`         |

## Contributing

If you'd like to contribute a module or fix an issue, please open a pull request or start a discussion in the issues tab.
