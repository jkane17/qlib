
/ 
    Package Manager
\

.pkg.priv.pkgs:([name:"s"$()] 
    loaded:"b"$(); updTime:"p"$(); file:"s"$(); depends:(); description:()
 );

// String form of function used to load packages.
.pkg.priv.ldStr:".pkg.load";
// Handle where error messages are to be written.
.pkg.priv.stderr:-2i;

.pkg.internal.path.qlib:.priv.qlib;
.pkg.internal.path.src:.Q.dd[.pkg.internal.path.qlib;`src];
.pkg.internal.path.cnf:.Q.dd[.pkg.internal.path.qlib;`cnf];
.pkg.internal.path.test:.Q.dd[.pkg.internal.path.qlib;`test];
.pkg.internal.path.unit:.Q.dd[.pkg.internal.path.test;`unit];
.pkg.priv.lib:.Q.dd[.pkg.internal.path.src;`lib];

// @brief Get a list of all the files in the given directory.
// @param dir : FileSymbol : Directory to get files from.
// @return Symbols : File names.
.pkg.priv.getFiles:{[dir] 
    if[0>type k:key dir; :`$()];
    files:.Q.dd[dir;] each k where b:(k:key dir) like "*.q";
    files,raze (.z.s .Q.dd[dir;]@) each k where not b
 };

// @brief Get name of file without directory path or file extension.
// @param file : FileSymbol : File path.
// @return Symbol : File name.
.pkg.priv.getName:{[file] first (`q<>last@){` vs last x}/file};

// @brief Parse a string to get package names.
// @param str : String : String to parse.
// @return Symbol : Package name.
.pkg.priv.getPkgNames:{[str] s:str?"`"; e:str?";"; value (e-s)#s _ str};

// @brief Parse a line from a package file to find its dependencies.
// @param str : String : String to parse.
// @param s   : Long   : Start index.
// @return Strings : Package names .
.pkg.priv.findDependencies:{[str;s] 
    raze .pkg.priv.getPkgNames each (s+count .pkg.priv.ldStr)_str
 };

// @brief Get dependencies for this package.
// @param file : FileSymbol : File path.
// @return Symbols : Other package dependencies for the given package file.
.pkg.priv.getDependencies:{[file]
    starts:ss[;.pkg.priv.ldStr] each raw:read0 file;
    i:where 0<count each starts;
    raze .pkg.priv.findDependencies'[raw i;starts i]
 };

// @brief Get the package dscription.
// @param file : FileSymbol : File path.
// @return String : Package description.
.pkg.priv.getDescription:{[file] ltrim 1_#[;r]?[;"\\"]r:raze read0 first file};

// @brief Get any cyclic dependencies.
// @return Dict : Map of package to its cyclic dependencies.
.pkg.priv.getCyclicDeps:{[]
    deps:exec name!depends from .pkg.priv.pkgs;
    p:{(any count each x) and not any key[x] in' value x};
    p {(distinct raze@) each x y}[deps;]/ deps
 };

// @brief Print an error message about missing packages.
// @param p : Symbol : Package name.
// @param m : Bools  : Missing dependencies in depends column of packages table.
.pkg.priv.printMissing:{[p;m]
    missing:@[;where m] exec first depends from .pkg.priv.pkgs where name=p;
    .pkg.priv.stderr "Dependencies in ",string[p]," package do not exist: `",
        "`" sv string missing;
 };

// @brief Update the loaded column in packages table.
// @param p : Symbol : Package name.
.pkg.priv.updLoaded:{[p] 
    update loaded:1b, updTime:.z.p from `.pkg.priv.pkgs where name=p;
 };

// @brief Ensure there are no missing and no cyclic dependencies.
.pkg.priv.validateDependencies:{[]
    allPkgs:.pkg.internal.allPkgs[];

    // No missing dependecies
    hasPkg:exec name!depends in allPkgs from .pkg.priv.pkgs;
    if[count pkgs:where not all each hasPkg;
        .pkg.priv.printMissing'[pkgs;not hasPkg pkgs];
        exit 1
    ];
    
    // No cyclic dependencies
    if[any count each d:.pkg.priv.getCyclicDeps[];
        pkgs:k where (k:key d) in' value d;
        .pkg.priv.stderr "Cyclic dependency detected between: `",
            "`" sv string pkgs;
        exit 1
    ]
 };

// @brief Initialise the package manager.
.pkg.priv.init:{[]
    files:.pkg.priv.getFiles .pkg.priv.lib;
    c:count files;
    `.pkg.priv.pkgs upsert flip (
        .pkg.priv.getName each files;
        c#0b;
        c#.z.p;
        files;
        .pkg.priv.getDependencies each files;
        .pkg.priv.getDescription each files
    );
    .pkg.priv.updLoaded `pkg;
    .pkg.priv.validateDependencies[];
 };

// @brief Get the path to a package file.
// @param p : Symbol : Package name.
// @return FileSymbol : Path to package file.
.pkg.priv.getFile:{[p] exec first file from .pkg.priv.pkgs where name=p};

// @brief Has a package been loaded?
// @param p : Symbol : Package name.
// @return Bool : 1b if pacakge has been loaded, 0b otherwise.
.pkg.priv.loaded:{[p] exec first loaded from .pkg.priv.pkgs where name=p};

// @brief Is a package valid?
// @param p : Symbol : Package name.
// @return Bool : 1b if package is valid, 0b otherwsie.
.pkg.priv.valid:{[p] p in .pkg.internal.allPkgs[]};

// @brief Check if a package is valid. Signal an error if not.
// @param p : Symbol : Package name.
.pkg.priv.validate:{[p] 
    if[not .pkg.priv.valid p; '"Error: Invalid Package - ",string p]
 };

// @brief Load a package.
// @param p : Symbol : Package name.
.pkg.priv.ld:{[p] system "l ",1_string .pkg.priv.getFile p;};

// @brief Get all package names.
// @return Symbols : All package names.
.pkg.internal.allPkgs:{[] exec name from .pkg.priv.pkgs};

// @brief Reload a package.
// @param p : Symbol : Package name.
// @return Symbols : Packages that were loaded (includes dependencies).
.pkg.reload:{[p]
    .pkg.priv.validate p;
    updTimes:exec updTime from .pkg.priv.pkgs;
    .pkg.priv.ld p;
    .pkg.priv.updLoaded p;
    exec name from .pkg.priv.pkgs where updTimes<>updTime
 };

// @brief Load a package.
// @param p : Symbols : Package names.
// @return Symbols : Packages that were loaded (includes dependencies).
.pkg.load:{[p] 
    $[all b:.pkg.priv.loaded each p,:();
        `$();
        raze .pkg.reload each p where not b
    ]
 };

.pkg.priv.init[];
