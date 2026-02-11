
/
    @file
        f.q
    
    @description
        File System Operations.
\

// @brief Get the basename of the given path.
// @param x fileSymbol Path to get basename of.
// @return symbol Basename.
basename:last ` vs;

// @brief Get the basename of the given path without the file extension.
// @param x fileSymbol Path to get basename of.
// @return symbol Basename without the file extension.
basenameNoExt:first ` vs basename@;

// @brief Get the directory of the given path.
// @param x fileSymbol Path to get directory name of.
// @return fileSymbol Directory name.
dirname:first ` vs;

// @brief Check if a given file/directory exists.
// @param path fileSymbol Path of a file/directory.
// @return boolean 1b if path exists, 0b otherwise.
exists:{[path] not ()~key path};

// @brief Recursively list all files and sub-directories within a given directory.
// @param dir fileSymbol Top level directory whose contents are to be listed.
// @return fileSymbols Relative list of paths to files and sub-directories.
rlist:{[dir] 
    $[
        not exists dir; `$(); 
        count p:.Q.dd[dir;] each key dir; raze p,'.z.s each p; 
        `$()
    ]
 };

// @brief Create a directory.
// @param dir fileSymbol Directory to create.
mkdir:{[dir] system $[isWindows; "mkdir "; "mkdir -p "],toPlatformPath dir;};

// @brief Foreceful and recursive removal of a file/directory.
// @param dir fileSymbol Path to file/directory to remove.
rmrf:{[path] if[exists path; hdel each desc path,rlist path]}

// @brief Copy a source file to a destination file.
// @param src fileSymbol File to copy.
// @param dst fileSymbol Location to copy to.
copy:{[src;dst] 
    mkdir dirname dst; // Ensure parent directory already exists
    system $[isWindows; "copy /v /z "; "cp "]," " sv toPlatformPath each src,dst;
 };

// @brief Recursively copy a source directory to a destination directory.
// @param src fileSymbol File to copy.
// @param dst fileSymbol Location to copy to.
rcopy:{[src;dst] 
    files:rlist src;
    copy'[files;] `$(string[dst],count[string src]_) each string files;
 };

// @brief Get the path to the default temporary directory.
// @return fileSymbol Path to temporary directory.
tmpdir:{[]
    hsym (^/)`$$[isWindows; 
        getenv[`TEMP`TMP],enlist $[count u:getenv`USERPROFILE;
            u,"\\AppData\\Local\\Temp";
            "C:\\Windows\\Temp"
        ];
        getenv[`TMPDIR`TMP`TEMP],enlist "/tmp"
    ]
 };

// @brief Convert a file path to a correctly formatted string path based on the platform.
// @param path fileSymbol File path to format.
// @return String Converted file path.
toPlatformPath:{[path]
    path:string path;
    if[isWindows; path[where"/"=path]:"\\"];
    (":"=first path)_ path
 };

export:([basename; basenameNoExt; mkdir; rmrf; copy; rcopy; tmpdir; toPlatformPath]);


isWindows:.z.o in `w32`w64;
