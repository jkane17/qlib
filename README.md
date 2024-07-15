
# qlib

Library for the Q programming language.

# Installing

1) Clone this repository 
    ```
    git clone <link>
    ```

2) Created the *QLIB* enviroment variable.

    ## Linux
    ```bash
    # Set QLIB in a start-up script
    # For example, in bash
    $ echo "export QLIB=/home/jkane/repos/qlib" >> /home/jkane/.bashrc

    # Source the script to define the variable in your current session
    $ source ~/.bashrc
    ``` 

    ## Windows

    ```
    TODO
    ```

    ## Mac OS

    ```
    TODO
    ```

3) Automate loading the q package manager when a q session is started. 

    Create a `q.q` file in $QHOME (if not previously create).
    ```bash
    # For example, in bash
    touch $QHOME/q.q
    ```

    Add the following lines to your `q.q` file
    ```q
    .priv.qlib:hsym `${$["~"=first x;getenv[`HOME],1_x;x]} getenv`QLIB;
    {if[not ()~key f:` sv .priv.qlib,`$"src/pkg.q"; system "l ",1_string f]}[];
    ```
