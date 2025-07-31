
/
    @file
        module.q
    
    @description
        Module management.
\

///// PUBLIC /////

// @brief Load module(s).
// @param modules Symbol|Symbols Module name(s).
// @return Symbols Modules that were loaded (includes dependencies).
.qlib.module.load:{[modules] 
    $[all loaded:.qlib.module.isLoaded each modules,:(); 
        `$(); 
        raze .qlib.module.reload each modules where not loaded
    ]
 };

// @brief Reload a module.
// @param module Symbol Module name.
// @return Symbols Modules that were (re)loaded (includes dependencies).
.qlib.module.reload:{[module]
    .qlibp.module.validate module;
    updTimes:exec updTime from .qlibp.module.modules;
    .qlibp.module.load module;
    .qlibp.module.markLoaded module;
    exec name from .qlibp.module.modules where updTimes<>updTime
 };

// @brief Has a module been loaded?
// @param module Symbol Module name.
// @return Bool 1b if module has been loaded, 0b otherwise.
.qlib.module.isLoaded:{[module] exec first loaded from .qlibp.module.modules where name=module};

// @brief List all module names.
// @return Symbols All module names.
.qlib.module.listAll:{[] exec name from .qlibp.module.modules};


///// PRIVATE /////

.qlibp.module.modules:([name:"s"$()] 
    loaded:"b"$(); updTime:"p"$(); file:"s"$(); depends:(); description:()
 );

// String form of function used to load modules.
.qlibp.module.loadFuncString:".qlib.module.load";

// Handle where error messages are to be written.
.qlibp.module.stderr:-2i;

// @brief Initialise the module manager.
.qlibp.module.init:{[]
    files:.qlibp.module.getFiles .qlibi.path.qlib;
    names:.qlibp.module.getName each files;
    files@:i:where names<>`module;
    names@:i;
    c:count files;
    `.qlibp.module.modules upsert flip (
        names;
        c#0b;
        c#.z.p;
        files;
        .qlibp.module.getDependencies each files;
        .qlibp.module.getDescription each files
    );
    .qlibp.module.validateDependencies[];
 };

// @brief Check if a module is valid. Signal an error if not.
// @param m Symbol Module name.
.qlibp.module.validate:{[m] if[not .qlibp.module.valid m; '"Error: Invalid Module - ",string m]};

// @brief Is a module valid?
// @param m Symbol Module name.
// @return Boolean 1b if module is valid, 0b otherwsie.
.qlibp.module.valid:{[m] m in .qlib.module.listAll[]};

// @brief Load a module.
// @param m Symbol Module name.
.qlibp.module.load:{[m] system "l ",1_string .qlibp.module.getFile m;};

// @brief Update the loaded column in modules table.
// @param m Symbol Module name.
.qlibp.module.markLoaded:{[m] 
    update loaded:1b, updTime:.z.p from `.qlibp.module.modules where name=m;
 };

// @brief Get the path to a module file.
// @param m Symbol Module name.
// @return FileSymbol Path to module file.
.qlibp.module.getFile:{[m] exec first file from .qlibp.module.modules where name=m};

// @brief List all the files in a given directory.
// @param dir FileSymbol Directory to list.
// @return Symbols File names.
.qlibp.module.getFiles:{[dir] 
    if[0>type k:key dir; :`$()];
    files:.Q.dd[dir;] each k where b:(k:key dir) like "*.q";
    files,raze (.z.s .Q.dd[dir;]@) each k where not b
 };

// @brief Get name of file without directory path or file extension.
// @param file FileSymbol File path.
// @return Symbol File name.
.qlibp.module.getName:{[file] first (`q<>last@){` vs last x}/file};

// @brief Get dependencies for the given module.
// @param file FileSymbol File path.
// @return Symbols Dependencies for the given module file.
.qlibp.module.getDependencies:{[file]
    starts:ss[;.qlibp.module.loadFuncString] each raw:read0 file;
    i:where 0<count each starts;
    raze .qlibp.module.findDependencies'[raw i;starts i]
 };

// @brief Parse a line from a module file to find its dependencies.
// @param str String String to parse.
// @param s Long Start index.
// @return Strings Module names.
.qlibp.module.findDependencies:{[str;s] 
    raze .qlibp.module.getModuleNames each (s+count .qlibp.module.loadFuncString)_str
 };

// @brief Parse a string to get module names.
// @param str String String to parse.
// @return Symbol Module name.
.qlibp.module.getModuleNames:{[str] s:str?"`"; e:str?";"; value (e-s)#s _ str};

// @brief Get the module dscription.
// @param file FileSymbol File path.
// @return String Module description.
.qlibp.module.getDescription:{[file] 
    trim 12_(first ss[r;"@description"]) _ r:#[;r]?[;"\\"] r:raze read0 first file
 };

// @brief Ensure there are no missing or cyclic dependencies.
.qlibp.module.validateDependencies:{[]
    allModules:.qlib.module.listAll[];

    // No missing dependencies
    hasModule:exec name!depends in allModules from .qlibp.module.modules;
    if[count modules:where not all each hasModule;
        .qlibp.module.printMissing'[modules;not hasModule modules];
        '"Missing module dependencies"
    ];
    
    // No cyclic dependencies
    if[any count each d:.qlibp.module.getCyclicDeps[];
        modules:k where (k:key d) in' value d;
        .qlibp.module.stderr "Cyclic dependency detected between: `",
            "`" sv string modules;
        exit 1
    ]
 };

// @brief Print an error message about missing modules.
// @param m Symbol Module name.
// @param d Booleans Missing dependencies in depends column of modules table.
.qlibp.module.printMissing:{[m;d]
    missing:@[;where d] exec first depends from .qlibp.module.modules where name=m;
    .qlibp.module.stderr "Dependencies in ",string[m]," module do not exist: `",
        "`" sv string missing;
 };

// @brief Get any cyclic dependencies.
// @return Dict Map of module to its cyclic dependencies.
.qlibp.module.getCyclicDeps:{[]
    deps:exec name!depends from .qlibp.module.modules;
    p:{(any count each x) and not any key[x] in' value x};
    p {(distinct raze@) each x y}[deps;]/ deps
 };

.qlibp.module.init[];
