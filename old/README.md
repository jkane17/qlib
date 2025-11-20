
# QLib

*QLib* is a modular standard library for the [Q programming language](https://code.kx.com/q/). It organises reusable functionality into logically grouped modules under a unified namespace.

## Features

* Organised by modules with automatic dependency management.
* Isolated namespace to prevent conflicts with user-defined symbols.
* Simple installation and integration with the q environment.

## Modules

*QLib* organises functionality into modules. Each module:

* Can depend on other modules.
* Automatically loads its dependencies.
* Exposes public values in a consistent namespace.

## Namespace

All *QLib* definitions live under the root namespace `.qlib.`:

    .qlib.<module>.<value>

For example, a logging function:

    .qlib.log.info

This design keeps user code clean and avoids symbol clashes. Users are expected not to define values inside `.qlib`.

## Private/Internal Values

QLib defines two categories of non-public values:

| Prefix | Description |
| - | - |
| `qlibp` | Private values intended for use only inside the module’s implementation file. |
| `qlibi` | Internal values that may be used across modules but not by external *QLib* users. |

These identifiers help signal intended usage boundaries.

## Installing

Clone this repository 

    ```
    git clone <LINK>
    ```

    Replace <LINK> with the actual repository URL.

## Building

The code can be built using a bash script located in the root directory of *QLib*:

```bash
$ ./build.sh
```

## Initialising

To initialise *QLib*, set the variable `.qlib.root` as the absolute path to the root of the `qlib` directory and load the `init.q` script:

```q
.qlib.root:`:/path/to/qlib;
system "l ",1_string .Q.dd[.qlib.root;`init.q];
```

## Contributing

If you'd like to contribute a module or fix an issue, please open a pull request or start a discussion in the issues tab.
