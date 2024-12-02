
/
    File:
        log.q
    
    Description:
        Logging utilities.
\

.pkg.load `escCode;

.log.priv.color:.pkg.internal.getCnfMap[`logColors;"ss"];
.log.internal.lvls:key .log.priv.color;

// Message to be logged if given level is invalid.
.log.priv.invalidMsg:"Invalid log level";

// @brief Symbolic name of a log level.
// @param x Long Log level value.
// @return Symbol Symbolic name.
.log.priv.name:.log.internal.lvls@;

// @brief Get the numeric value of a level.
// @param x Symbol Log level.
// @return Long Numeric value.
.log.priv.val:.log.internal.lvls?upper@;

// @brief Is the given level active.
// @param lvl Symbol Log level.
// @return Bool 1b if active, 0b otherwise.
.log.priv.active:{[lvl] .log.priv.lvl<=.log.priv.val lvl};

// @brief Is the given level ERROR or FATAL.
// @param lvl Symbol Log level.
// @return Bool 1b if ERROR or FATAL, 0b otherwise.
.log.priv.iserr:.log.priv.val[`ERROR]<=.log.priv.val@;

// @brief Log file handle.
// @param lvl Symbol Log level.
// @return Int Handle to log file.
.log.priv.hdl:{[lvl] 
    neg $[.log.priv.iserr lvl;.log.priv.stderr;.log.priv.stdout]
 };

// @brief Add meta data to the log msg (time, level, etc.).
// @param lvl Symbol Log level.
// @param msg String Message to be logged.
// @return String Message with meta prepended.
.log.priv.prependMeta:{[lvl;msg]
    lvlText:.escCode.apply[.log.priv.color lvl;"[",string[lvl],"]"];
    string[.z.p]," ",lvlText," : ",msg
 };

// @brief Write a message to all handles if the given level is active.
// @param lvl Symbol Log level.
// @param msg String Message to be logged.
// @return String Message that was logged.
.log.priv.write:{[lvl;msg] 
    if[.log.priv.metaEnabled; msg:.log.priv.prependMeta[lvl;msg]];
    .log.priv.hdl[lvl] msg;
    msg
 };

// @brief Define the main logging functions based on the active level.
.log.priv.defineFuncs:{[]
    {.log[lower x]:$[.log.priv.active x;.log.priv.write[x;];(::)]} each -1_.log.internal.lvls;
 };

// @brief Is the given level valid.
// @param lvl Symbol Log level.
// @return Bool 1b if valid level, 0b otherwise.
.log.internal.valid:in[;.log.internal.lvls] upper@;

// @brief Set the stdout file handle.
// @param x Short|Int|Long Handle to set.
.log.setStdout:{.log.priv.stdout:"i"$abs x;};

// @brief Get the current stdout file handle.
// @return Int Stdout file handle.
.log.getStdout:{[] .log.priv.stdout};

// @brief Set the stderr file handle.
// @param x Short|Int|Long Handle to set.
.log.setStderr:{.log.priv.stderr:"i"$abs x;};

// @brief Get the current stderr file handle.
// @return Int Stderr file handle.
.log.getStderr:{[] .log.priv.stderr};

// @brief Prepend meta info to logged message.
.log.enableMeta:{[] .log.priv.metaEnabled:1b;};

// @brief Do not prepend meta info to logged message.
.log.disableMeta:{[] .log.priv.metaEnabled:0b;};

// @brief Is meta prepending enabled?
// @return Bool 1b if meta prepending is enabled, 0b otherwise.
.log.isMetaEnabled:{[] .log.priv.metaEnabled};

// @brief Set the current log level.
// @param lvl Symbol Log level.
.log.setLvl:{[lvl] 
    $[.log.internal.valid lvl; 
        [.log.priv.lvl:.log.priv.val lvl; .log.priv.defineFuncs[]]; 
        '.log.priv.invalidMsg
    ]; 
 };

// @brief Get the current log level..
// @return Symbol Current log level.
.log.getLvl:{[] .log.priv.name .log.priv.lvl};

///// Main logging functions. Definition controlled by .log.priv.defineFuncs /////

// @brief Log trace message.
// @param msg String Message to be logged.
// @return String Logged message.
.log.trace:(::);

// @brief Log deug message.
// @param msg String Message to be logged.
// @return String Logged message.
.log.debug:(::);

// @brief Log info message.
// @param msg String Message to be logged.
// @return String Logged message.
.log.info:(::);

// @brief Log warning message.
// @param msg String Message to be logged.
// @return String Logged message.
.log.warn:(::);

// @brief Log error message.
// @param msg String Message to be logged.
// @return String Logged message.
.log.error:(::);

// @brief Log fatal message.
// @param msg String Message to be logged.
// @return String Logged message.
.log.fatal:(::);

///// /////

// Set defaults
.log.setStdout 1i;
.log.setStderr 2i;
.log.setLvl `INFO;
.log.enableMeta[];
