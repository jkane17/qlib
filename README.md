
# QLib

Library for the Q programming language.

# Installing

1) Clone this repository 
    ```
    git clone <link>
    ```

2) Create the *QLIB* and *QEXE* environment variables. Note that you should replace *USER* and *PLATFORM* (w64, l64, etc.) as appropriate.

    ## Linux/macOS Terminal

    ```bash
    # Set QLIB and QEXE in a start-up script
    # For example, in bash
    $ echo 'export QLIB=${HOME}/repos/qlib' >> ${HOME}/.bashrc
    $ echo 'export QEXE=${QHOME}/PLATFORM/q' >> ${HOME}/.bashrc

    # Source the script to define the variables in your current session
    $ source ~/.bashrc
    ``` 

    ## Windows Command Prompt

    ```powershell
    setx QLIB "$env:USER\repos\qlib"
    setx QEXE "$env:QHOME\PLATFORM\q"
    ```

3) Automate loading the q package manager when a q session is started. 

    * Create a `q.q` file in `QHOME` (if not previously created).
        
        ## Linux/macOS Terminal

        ```bash
        # For example, in bash
        touch $QHOME/q.q
        ```

        ## Windows Command Prompt

        ```powershell
        echo. > %QHOME%\q.q
        ```

    * Add the following lines to your `q.q` file:

        ```q
        .priv.qlib:hsym `${$["~"=first x;getenv[`HOME],1_x;x]} getenv`QLIB;
        {if[not ()~key f:` sv .priv.qlib,`$"src/pkg.q"; system "l ",1_string f]}[];
        ```
