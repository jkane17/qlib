
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

1. Clone this repository 

    ```
    git clone <LINK>
    ```

    Replace <LINK> with the actual repository URL.

2. Set the `QLIB` Environment Variable

    ### Linux/macOS

    ```bash
    # Add to your shell configuration (e.g., .bashrc, .zshrc)
    echo 'export QLIB=$HOME/repos/qlib' >> ~/.bashrc

    # Apply changes
    source ~/.bashrc
    ``` 

    ### Windows

    ```powershell
    setx QLIB "$env:USERPROFILE\repos\qlib"
    ```

3. Auto-load QLib on Q Startup

    a. Create a `q.q` file in `QHOME` directory (if it doesn't exist)
        
    ## Linux/macOS

    ```bash
    touch $QHOME/q.q
    ```

    ## Windows

    ```powershell
    echo. > %QHOME%\q.q
    ```

    b. Add the following to `q.q`

    ```q
    // Load QLib on session start
    {
        e:"[ERROR] Failed to load QLib: ";
        r:hsym `${$["~"=first x;getenv[`HOME],1_x;x]} getenv`QLIB;
        if[()~key f:.Q.dd[r;`init.q]; -2 e,"Missing file ",1_string f; :()];
        .qlibi.path.root:r;
        @[system; "l ",1_string f; 
            {[e;x] {delete from x} each `.qlib`.qlibi`.qlibp;  -2 .qlib.error:e,x}[e;]
        ];
     }[];
    ```

## Contributing

If you'd like to contribute a module or fix an issue, please open a pull request or start a discussion in the issues tab.
