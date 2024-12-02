
/
    File:
        os.q
    
    Description:
        System commands.
\

.pkg.load `cast`flt`fstr`log`util;

// Q Processes
/ .os.priv.procs:([pid:"i"$()] port:);

.os.priv.osMap:(`$"lmsvw" cross ("32";"64"))!raze 2#/:`lnx`mac`sol`sli`win;
.os.priv.osMap[`l64arm]:`lnx;
.os.priv.os:.os.priv.osMap .z.o;

.os.priv.cmds:exec name!cmd from .pkg.internal.getCnfTable[`os;"ss*"] where os=.os.priv.os;
.os.priv.stderrRedirect:$[.os.priv.os~`win;"2>nul";"2>/dev/null"];
.os.priv.hiddenStderr:0b;

// @brief Parse a command and its arguments.
// @param cmd Symbol Command name.
// @return String System command.
.os.priv.getCmd:{[cmd] 
    $[count r:.os.priv.cmds cmd;
        r;
        '.log.error .fstr.fmt["Unknown command: {}";cmd]
    ]
 };

// @brief Parse a system command and its arguments.
// @param cmd Symbol Command name.
// @param args Any Arguments given to the command.
// @return String System command.
.os.priv.parseCmd:{[cmd;args]
    ty:type args;
    if[(ty<0h) or ty=10h; args:enlist args];
    cmd:.fstr.fmt[.os.priv.getCmd cmd;args];
    cmd
 };

// @brief Ensure a path has the appropriate separator based on the OS.
// @param path FileSymbol|Symbol|String Path of a file/directory.
// @return String Converted path.
.os.priv.convertPath:{[path] $[.os.priv.os~`win; ssr[;"/";"\\"]; ssr[;"\\";"/"]] .cast.htostr path};

// @brief Run a system command.
// @param cmd String|Symbol Command to execute.
// @param args Any Arguments given to the command.
// @return String Command output.
.os.priv.run:{[cmd;args]
    if[-11h=type cmd; cmd:.os.priv.parseCmd[cmd;args]];
    .log.debug "Running system command: ",cmd;
    system cmd
 };

// @brief Apply a command that takes a source and destination as its arguments.
// @param cmd Symbol Command name.
// @param src FileSymbol|Symbol|String Path to source.
// @param dst FileSymbol|Symbol|String Path to destination.
.os.priv.srcToDst:{[cmd;src;dst] .os.priv.run[cmd;] .os.priv.convertPath each (src;dst);};

// @brief Copy src to dst.
// @param src FileSymbol|Symbol|String Path to source.
// @param dst FileSymbol|Symbol|String Path to destination.
.os.cp:{[src;dst] .os.priv.srcToDst[`cp;src;dst];};

// @brief Check if a given file/directory exists.
// @param path FileSymbol|Symbol|String Path of a file/directory.
// @return Boolean 1b if path exists, 0b otherwise.
.os.exists:{[path] not ()~key .cast.tohsym path};

// @brief Check if stderr output is hidden.
// @param hide Boolean 1b to hide stderr output, false otherwise.
.os.hideStderr:{[hide] 
    if[hide<>.os.priv.hiddenStderr; 
        .os.priv.cmds:$[hide;
            .os.priv.cmds,\:" ",.os.priv.stderrRedirect;
            (neg 1+count .os.priv.stderrRedirect)_/:.os.priv.cmds
        ];
        .os.priv.hiddenStderr:hide
    ]
 };

// @brief Check if stderr output is hidden.
// @return Boolean 1b if stderr output is hidden, 0b otherwise.
.os.hiddenStderr:{[] .os.priv.hiddenStderr};

// @brief Create a directory.
// @param dir FileSymbol|Symbol|String Path of directory to create.
// @return FileSymbol Path to created directory.
.os.mkdir:{[dir] 
    if[not .os.exists dir; .os.priv.run[`mkdir;] .os.priv.convertPath dir];
    dir
 };

// @brief Move/rename src to dst.
// @param src FileSymbol|Symbol|String Path to source.
// @param dst FileSymbol|Symbol|String Path to destination.
.os.mv:{[src;dst] .os.priv.srcToDst[`mv;src;dst];};

// @brief Recursively list all files and sub-directories within a given directory.
// @param dir FileSymbol Top level directory whose contents are to be listed.
// @return FileSymbols Relative list of paths to files and sub-directories.
.os.rls:{[dir] 
    $[not .os.exists dir; (); count p:.Q.dd[dir;] each key dir; raze p,'.z.s each p; `$()]
 };

// @brief Remove a file/directory.
// @param path FileSymbol|Symbol|String Path of a file/directory to remove.
.os.rm:{[path] if[.os.exists path; hdel path]};

// @brief Foreceful and recursive removal of a file/directory.
// @param path FileSymbol|Symbol|String Path of a file/directory to remove.
.os.rmrf:{[path] if[.os.exists path; hdel each desc path,.os.rls path]};

// @brief Run a raw system command.
// @param cmd String Command to execute.
// @return String Command output.
.os.run:{[cmd] .os.priv.run[cmd;()]};

// @brief Load a file into a Q session.
// @param file : FileSymbol|Symbol|String : File to load.
.os.load:{[file] .os.run 1_.cast.htostr file};

/

// @brief Run a raw system command in the background.
// @param cmd : String : Command to execute.
.os.runBg:{[cmd] 
    pid:"I"$first .os.priv.run[`bg;cmd];

 };

// @brief Find the port given the process ID.
.os.portByPID:{[pid]
    r:.os.run .fstr.fmt["lsof -p {} | grep LISTEN | awk '{ print $9; }'";pid];
    port:distinct .flt.notNull "I"$(last ":" vs) each r;
    if[1<>count port; '"Could not find port"];
    first port
 };

cmd:"/home/jkane/q/l64/q -p 5001 -w 2000 -arg1 123 -q";
.os.portByPID pid
