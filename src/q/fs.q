
/
    @file fs.q
    
    @overview
        Functions for performing common file system operations.
\

// @brief Get the basename of the given path.
// @param path {symbol|fileSymbol} Path to get basename of.
// @return {symbol} Basename.
basename:{[path:`s] last ` vs hsym path};

// @brief Get the basename of the given path without the file extension.
// @param path {symbol|fileSymbol} Path to get basename of.
// @return {symbol} Basename without the file extension.
basenameNoExt:{[path:`s] first ` vs basename path};

// @brief Copy a source file to a destination file.
// @param src {fileSymbol} File to copy.
// @param dst {fileSymbol} Location to copy to.
copy:{[src:`s;dst:`s] 
    mkdir dirname dst; // Ensure parent directory already exists
    system $[isWindows; "copy /v /z "; "cp "]," " sv toPlatformPath each src,dst;
 };

// @brief Get the directory of the given path.
// @param path {fileSymbol} Path to get directory name of.
// @return {fileSymbol} Directory name.
dirname:{[path:`s] first ` vs path};

// @brief Check if a given file/directory exists.
// @param path {fileSymbol} Path of a file/directory.
// @return {boolean} `1b` if the path exists, `0b` otherwise.
exists:{[path:`s] not ()~key path};

// @brief Check if a given path is a directory.
// @param path {fileSymbol} Path to check.
// @return {boolean} `1b` if path is a directory, `0b` otherwise.
isdir:{[path:`s] 11h=type key path};

// @brief Check if a given path is a file.
// @param path {fileSymbol} Path to check.
// @return {boolean} `1b` if path is a file, `0b` otherwise.
isfile:{[path:`s] -11h=type key path};

// @brief Create a directory.
// @param dir {fileSymbol} Directory to create.
mkdir:{[dir:`s] system $[isWindows; "mkdir "; "mkdir -p "],toPlatformPath dir;};

// @brief Move/rename a source file to a destination file.
// @param src {fileSymbol} File to move/rename.
// @param dst {fileSymbol} Location to move/rename to.
move:{[src:`s;dst:`s] 
    mkdir dirname dst; // Ensure parent directory already exists
    system $[isWindows; "move "; "mv "]," " sv toPlatformPath each src,dst;
 };

// @brief Recursively copy a source directory to a destination directory.
// @param src {fileSymbol} File to copy.
// @param dst {fileSymbol} Location to copy to.
rcopy:{[src:`s;dst:`s] 
    files:rlist src;
    copy'[files;] `$(string[dst],count[string src]_) each string files;
 };

// @brief List all top level sub-directories within a given directory.
// @param dir {fileSymbol} Directory whose sub-directories are to be listed.
// @return {fileSymbol[]} Relative list of paths to sub-directories.
listd:{[dir:`s] 
    $[exists dir;
        p where isdir each p:.Q.dd[dir;] each key dir;
        `$()
    ]
 };

// @brief List all top level files within a given directory.
// @param dir {fileSymbol} Directory whose files are to be listed.
// @return {fileSymbol[]} Relative list of paths to files.
listf:{[dir:`s] 
    $[exists dir;
        p where isfile each p:.Q.dd[dir;] each key dir;
        `$()
    ]
 };

// @brief Recursively list all files and sub-directories within a given directory.
// @param dir {fileSymbol} Top level directory whose contents are to be listed.
// @return {fileSymbol[]} Relative list of paths to files and sub-directories.
rlist:{[dir:`s] 
    $[
        not exists dir; `$(); 
        count p:.Q.dd[dir;] each key dir; raze p,'.z.s peach p; 
        `$()
    ]
 };

// @brief Recursively list all files within a given directory.
// @param dir {fileSymbol} Top level directory whose contents are to be listed.
// @return {fileSymbol[]} Relative list of paths to files.
rlistf:{[dir:`s] 
    $[
        11h=t:type key dir; raze (.z.s .Q.dd[dir;]@) peach key dir;
        -11h=t; dir;
        `$()
    ]
 };

// @brief Foreceful and recursive removal of a file/directory.
// @param path {fileSymbol} Path to file/directory to remove.
rmrf:{[path:`s] if[exists path; hdel each desc path,rlist path]}

// @brief Get the path to the default temporary directory.
// @return {fileSymbol} Path to temporary directory.
tmpdir:{[]
    hsym (^/)`$$[isWindows; 
        getenv[`TEMP`TMP],enlist $[count u:getenv`USERPROFILE;
            u,"\\AppData\\Local\\Temp";
            "C:\\Windows\\Temp"
        ];
        getenv[`TMPDIR`TMP`TEMP],enlist "/tmp"
    ]
 };

// @brief Compute the size (in bytes) of a file/directory. 
// @param path {fileSymbol} Path to file/directory to compute size for.
// @return {long} Size of file/directory in bytes.
size:{[path:`s] $[.fs.isdir path; sum (.z.s .Q.dd[path;]@) peach key path; @[hcount;path;0]]};

// @brief Convert a file path to a correctly formatted string path based on the platform.
// @param path {fileSymbol} File path to format.
// @return {string} Converted file path.
toPlatformPath:{[path:`s]
    path:string path;
    if[isWindows; path[where"/"=path]:"\\"];
    (":"=first path)_ path
 };

export:([
    basename; basenameNoExt; copy; dirname; exists; isdir; isfile; mkdir; move; rcopy; listd;
    listf; rlist; rlistf; rmrf; tmpdir; toPlatformPath
 ]);


isWindows:.z.o in `w32`w64;
