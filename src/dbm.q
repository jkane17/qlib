
/
    @file
        dbm.q
    
    @description
        Database Maintenance.
\

fs:use`qlib.fs;

// @brief Add a column to a database table.
// @param db fileSymbol Path to database root.
// @param domain symbol Sym file (domain) name (only used if column is symbol type).
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param default any Default value of the column.
addCol:{[db:`s;domain:`s;tname:`s;cname:`s;default]
    validateName cname;
    default:enum[db;domain;default];
    add1Col[;cname;default] peach allTablePaths[db;tname];
 };

// @brief Add columns of matching types to a database table.
// @param db fileSymbol Path to database root.
// @param domain symbol Sym file (domain) name (only used if columns are symbol type).
// @param tname symbol Table name.
// @param cnames symbols Column names.
// @param default any Default value of the columns.
addCols:{[db:`s;domain:`s;tname:`s;cnames:`S;default]
    validateName each cnames;
    default:enum[db;domain;default];
    addCol[db;domain;tname;srcCol:first cnames;default];
    copyCol[db;tname;srcCol;]each 1_cnames;
 };

// @brief Add missing columns across all partitions of a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param goodTdir fileSymbol Path of a table directory which has no missing columns.
addMissingCols:{[db:`s;tname:`s;goodTdir:`s]
    add1MissingCols[;0#get goodTdir] peach allTablePaths[db;tname] except goodTdir;
 };

// @brief Add a new table to all partitions of a database.
// @param db fileSymbol Path to database root.
// @param domain symbol Sym file (domain) name.
// @param tname symbol New table name.
// @param schema table New table schema.
addTab:{[db:`s;domain:`s;tname:`s;schema] 
    add1Tab[db;domain;;schema] peach buildTablePaths[db;tname];
 };

// @brief Cast a column to a given type.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param typ short|char|symbol Type to cast column to.
castCol:{[db:`s;tname:`s;cname:`s;typ] fnCol[db;tname;cname;typ$];};

// @brief Copy a column across all partitions of a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param srcCol symbol Column name whose data will be copied.
// @param dstCol symbol New column name that will be created.
copyCol:{[db:`s;tname:`s;srcCol:`s;dstCol:`s] 
    validateName dstCol;
    copy1Col[;srcCol;dstCol] peach allTablePaths[db;tname];
 };

// @brief Delete a column from a database table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
delCol:{[db:`s;tname:`s;cname:`s] del1Col[;cname] peach allTablePaths[db;tname];};

// @brief Delete a table from a database.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
delTab:{[db:`s;tname:`s] del1Tab peach allTablePaths[db;tname];};

// @brief Build a mapping of domain name to indexes of unused symbols in the given database table.
// If a domain is not used at all, it will not show up in the result.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return dict Mapping of domain name to indexes of unused symbols.
domainUnused:{[db:`s;tname:`s] 
    except'[;used] (til count get db,) each key used:domainUsed[db;tname]
 };

// @brief Build a mapping of domain name to indexes of used symbols in the given database.
// @param db fileSymbol Path to database root.
// @return dict Mapping of domain name to indexes of used symbols.
domainUsage:{[db:`s] (union'/) domainUsed[db;] peach listTabs db};

// @brief Build a mapping of domain name to indexes of used symbols in the given database table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return dict Mapping of domain name to indexes of used symbols.
domainUsed:{[db:`s;tname:`s] (union'/) domainUsed1 peach allTablePaths[db;tname]};

// @brief Apply a function to a column across all partitions of a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param fn function Unary function to apply to the column.
fnCol:{[db:`s;tname:`s;cname:`s;fn] fn1Col[;cname;fn] peach allTablePaths[db;tname];};

// @brief Does the given column exist in the table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @return bool 1b if the column exists within the table, 0b otherwise.
hasCol:{[db:`s;tname:`s;cname:`s] 
    $[count paths:allTablePaths[db;tname]; all has1Col[;cname] peach paths; 0b]
 };

// @brief List all column names of the given table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return symbols Column names.
listCols:{[db:`s;tname:`s] getColNames mostRecentTdir[db;tname]};

// @brief List all enumeration (symbol) column names of the given table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return symbols Enumeration (symbol) column names.
listEnumCols:{[db:`s;tname:`s] where 20h=colTypes[db;tname]};

// @brief List all table names in a database.
// @NOTE Uses the most recent partition in a partitioned database.
// @param db fileSymbol Path to database root.
// @return symbols Table names.
listTabs:{[db:`s] 
    if[0=count files:key db; :`$()];
    if[any files like "par.txt"; :distinct raze .z.s each hsym `$read0 .Q.dd[db;`par.txt]];
    isPartition:files like "[0-9]*";
    tabs@:where (fs.isdir .Q.dd[db;]@) each tabs:files where not isPartition;
    if[count files@:where isPartition; tabs,:key .Q.dd[db;] max "D"$/:string files];
    asc tabs
 };

// @brief Save each domain as a file containing the symbol values.
// @param dir fileSymbol Path to persist to.
// @param dm dict Mapping of domain name to symbol values.
// @return dict Mapping of domain name to persisted path.
persistDomainMap:{[dir:`s;dm] (.Q.dd[dir;] each key dm) set' dm};

// @brief Recreate all domains of the given database with only the used symbols.
// @param db fileSymbol Path to database root.
// @param dir fileSymbol Path to persist to.
// @return dict Mapping of domain name to persisted path.
rebuildDomains:{[db:`s;dir] persistDomainMap[dir;] resolveDomainMap[db;] domainUsage db};

// @brief Re-enumerate every table in the database.
// @param db fileSymbol Path to database root.
// @param newDomainFile fileSymbol New domain file to re-enumerate against.
reenumerateAll:{[db:`s;newDomainFile:`s] reenumerateTab[db;;newDomainFile] peach listTabs db;};

// @brief Re-enumerate every table in the database from a given domain.
// @param db fileSymbol Path to database root.
// @param currDomainName symbol Name of the current domain.
// @param newDomainFile fileSymbol New domain file to re-enumerate against.
reenumerateAllFrom:{[db:`s;currDomainName:`s;newDomainFile:`s] 
    reenumerateTabFrom[db;;currDomainName;newDomainFile] peach listTabs db;
 };

// @brief Re-enumerate a column against a new domain across all partitions.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param newDomainFile fileSymbol New domain file to re-enumerate against.
reenumerateCol:{[db:`s;tname:`s;cname:`s;newDomainFile:`s]
    if[not null currDomainName:colDomainName[db;tname;cname];
        reenumerateCol0[db;tname;cname;currDomainName;newDomainFile]
    ];
 };

// @brief Re-enumerate a column from a specific domain to a new domain.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param currDomainName symbol Name of the current domain.
// @param newDomainFile fileSymbol New domain file to re-enumerate against.
reenumerateColFrom:{[db:`s;tname:`s;cname:`s;currDomainName:`s;newDomainFile:`s]
    if[currDomainName=colDomainName[db;tname;cname];
        reenumerateCol0[db;tname;cname;currDomainName;newDomainFile]
    ];
 };

// @brief Re-enumerate all enumeration columns in a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param newDomainFile fileSymbol New domain file to re-enumerate against.
reenumerateTab:{[db:`s;tname:`s;newDomainFile:`s]
    reenumerateCol[db;tname;;newDomainFile] peach listEnumCols[db;tname];
 };

// @brief Re-enumerate all enumeration columns in a table from a given domain.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param currDomainName symbol Name of the current domain.
// @param newDomainFile fileSymbol New domain file to re-enumerate against.
reenumerateTabFrom:{[db:`s;tname:`s;currDomainName:`s;newDomainFile:`s]
    reenumerateColFrom[db;tname;;currDomainName;newDomainFile] peach listEnumCols[db;tname];
 };

// @brief Rename a column across all partitions of a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param old symbol Current column name.
// @param new symbol New column name.
renameCol:{[db:`s;tname:`s;old:`s;new:`s] 
    validateName new;
    rename1Col[;old;new] peach allTablePaths[db;tname];
 };

// @brief Rename a (symbol) domain.
// @param db fileSymbol Path to database root.
// @param currName symbol Current domain name.
// @param newName symbol New domain name.
renameDomain:{[db:`s;currName:`s;newName:`s]
    currFile:.Q.dd[db;currName];
    newFile:.Q.dd[db;newName];

    fs.copy[currFile;newFile];
    reenumerateAllFrom[db;currName;newFile];
 };

// @brief Rename a table in all partitions.
// @param db fileSymbol Path to database root.
// @param old symbol Current table name.
// @param new symbol New table name.
renameTab:{[db:`s;old:`s;new:`s] 
    validateName new;
    .[rename1Tab;] peach flip buildTablePaths[db;] each old,new;
 };

// @brief Reorder the columns across all partitions of a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param order symbols New ordering of the columns (some or all).
reorderCols:{[db:`s;tname:`s;order:`S] reorder1Cols[;order] peach allTablePaths[db;tname];};

// @brief Convert a domain mapping from index values to symbol values.
// @param db fileSymbol Path to database root.
// @param dm dict Mapping of domain name to indexes.
// @return dict Mapping of domain name to symbols.
resolveDomainMap:{[db:`s;dm] ((get db,) each key dm)@'dm};

// @brief Remove an attribute from a column.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
rmAttr:{[db:`s;tname:`s;cname:`s] setAttr[db;tname;cname;`];};

// @brief Set an attribute on a column.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param attrb symbol Attribute (s, u, p, g).
setAttr:{[db:`s;tname:`s;cname:`s;attrb:`s] fnCol[db;tname;cname;attrb#];};

// @brief Convert a column from string type to symbol type.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param dname symbol Name of domain to enumerate against.
strToSymCol:{[db:`s;tname:`s;cname:`s;dname:`s]
    dfile:.Q.dd[db;dname];
    domain:strToSym1Col[;;cname;dname]/[get dfile;allTablePaths[db;tname]];
    dfile set domain;
 };

// @brief Convert a column from symbol type to string type.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
symToStrCol:{[db:`s;tname:`s;cname:`s]
    if[not null domainName:colDomainName[db;tname;cname];
        fnCol[db;tname;cname;string get[db,domainName]@]
    ];
 };


export:([
    addCol; addCols; addMissingCols; castCol; copyCol; delCol; delTab; domainUnused; domainUsage; 
    domainUsed; fnCol; hasCol; listCols; persistDomainMap; rebuildDomains; reenumerateAll;
    reenumerateAllFrom; reenumerateCol; reenumerateColFrom; reenumerateTab; reenumerateTabFrom; 
    renameCol; renameDomain; renameTab; reorderCols; resolveDomainMap; rmAttr; setAttr;
    strToSymCol; symToStrCol 
 ]);


// @brief Add a column to a splayed table.
// @param tdir fileSymbol Table directory.
// @param cname symbol Column name.
// @param default any Default value of the column.
add1Col:{[tdir:`s;cname:`s;default]
    if[not cname in colNames:getColNames tdir;
        len:count get tdir,first colNames;
        .[.Q.dd[tdir;cname];();:;len#default];
        @[tdir;`.d;,;cname]
    ]
 };

// @brief Add missing columns to a single database table
// @param tdir fileSymbol Table directory.
// @param good table Table which has no missing columns to be used as a template.
add1MissingCols:{[tdir:`s;good]
    goodCols:cols good;
    if[count missing:goodCols except getColNames tdir;
        {[d;g;c] add1Col[d;c;0#g c]}[tdir;good;] each missing;
        reorder1Cols[tdir;goodCols]
    ]
 };

// @param Add a single new table.
// @param db fileSymbol Path to database root.
// @param domain symbol Sym file (domain) name.
// @param tdir fileSymbol New table directory.
// @param schema table New table schema.
add1Tab:{[db:`s;domain:`s;tdir:`s;schema] @[tdir;`;:;.Q.ens[db;0#schema;domain]];};

// @brief Copy the data from an existing column in a table to a new column.
// @param tdir fileSymbol Table directory.
// @param srcCol symbol Column name whose data will be copied. 
// @param dstCol symbol New column name that will be created.
copy1Col:{[tdir:`s;srcCol:`s;dstCol:`s]
    if[(srcCol in colNames) and not dstCol in colNames:getColNames tdir;
        fs.copy . .Q.dd[tdir;] each srcCol,dstCol;
        if[(hname:`$string[srcCol],"#") in key tdir; 
            fs.copy . .Q.dd[tdir;] each hname,`$string[dstCol],"#"
        ];
        @[tdir;`.d;,;dstCol]
    ]
 };

// @brief Delete a column from a database table.
// @param tdir fileSymbol Table directory.
// @param cname symbol Name of column to be deleted.
del1Col:{[tdir:`s;cname:`s]
    if[cname in colNames:getColNames tdir;
        hdel .Q.dd[tdir;cname];
        if[(hname:`$string[cname],"#") in key tdir; hdel .Q.dd[tdir;hname]];
        @[tdir;`.d;:;colNames except cname]
    ]
 };

// @brief Delete a table directory and its contents.
// @param tdir fileSymbol Table directory to delete.
del1Tab:{[tdir:`s] if[not ()~files:key tdir; (hdel .Q.dd[tdir;]@) each files,`]};

// @brief Build a mapping of domain name to indexes of used symbols in the given splayed table.
// @param tdir fileSymbol Table directory.
// @return dict Mapping of domain name to indexes of symbols used.
domainUsed1:{[tdir]
    {[tdir;map;col]
        if[20h=type enum:get tdir,col; map[key enum]:map[key enum] union value enum];
        map
    }[tdir]/[([]);getColNames tdir]
 };

// @brief Apply a function to a single database table.
// @param tdir fileSymbol Table directory.
// @param cname symbol Column name.
// @param fn function Unary function to apply to the column.
fn1Col:{[tdir:`s;cname:`s;fn]
    if[has1Col[tdir;cname];
        oldAttr:attr oldVal:get tdir,cname;
        newAttr:attr newVal:fn oldVal;
        if[$[oldAttr~newAttr;not oldVal~newVal;1b];
            .[.Q.dd[tdir;cname];();:;newVal]
        ]
    ]
 };

// @brief Does the given column exist in the table.
// @param tdir fileSymbol Table directory.
// @param cname symbol Column name.
// @return bool 1b if the column exists within the table, 0b otherwise.
has1Col:{[tdir:`s;cname:`s] cname in getColNames tdir};

// @brief Re-enumerate a column against a new domain across all partitions.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @param currDomainName symbol Name of the current domain.
// @param newDomainFile fileSymbol New domain file to re-enumerate against.
reenumerateCol0:{[db;tname;cname;currDomainName;newDomainFile]
    currDomain:get db,currDomainName;
    newDomain:get newDomainFile;
    newDomainName:fs.basename newDomainFile;

    fnCol[db;tname;cname;] {[currDomain;newDomain;newDomainName;vals]
        newDomainName!.Q.fu[newDomain?;currDomain vals]
    }[currDomain;newDomain;newDomainName;];
 };

// @brief Rename a column in a single database table.
// @param tdir fileSymbol Table directory.
// @param old symbol Current column name.
// @param new symbol New column name.
rename1Col:{[tdir:`s;old:`s;new:`s]
    if[(old in colNames) and not new in colNames:getColNames tdir;
        fs.move . .Q.dd[tdir;] each old,new;
        if[(hname:`$string[old],"#") in key tdir; 
            fs.move . .Q.dd[tdir;] each hname,`$string[new],"#"
        ];
        @[tdir;`.d;:;.[colNames;where colNames=old;:;new]]
    ]
 };

// @brief Rename a table.
// @param old fileSymbol Path to current table within a partition.
// @param new fileSymbol Path to new table within a partition.
rename1Tab:{[old:`s;new:`s] if[()~key new; fs.move[old;new]]};

// @brief Reorder the columns in a single database table.
// @param tdir fileSymbol Table directory.
// @param order symbols New ordering of the columns.
reorder1Cols:{[tdir:`s;order:`S]
    if[not all exists:order in colNames:getColNames tdir;
        '"Unknown column(s): ","," sv string order where not exists
    ];
    @[tdir;`.d;:;order,colNames except order];
 };

// @brief Convert a column from string type to symbol type.
// @param domain symbols Symbol domain.
// @param tdir fileSymbol Table directory.
// @param cname symbol Column name.
// @param dname symbol Domain name.
// @return symbols Updated domain.
strToSym1Col:{[domain:`S;tdir:`s;cname:`s;dname:`s]
    if[has1Col[tdir;cname];
        domain:domain union vals:`$/:get cfile:.Q.dd[tdir;cname];
        .[cfile;();:;dname!.Q.fu[domain?;vals]]
    ];
    domain
 };

// @brief Get all existing paths to a table within a database.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return fileSymbols List of paths to table within database.
allTablePaths:{[db:`s;tname:`s] paths where 0<(count key@) each paths:buildTablePaths[db;tname]};

// @brief Build all paths to a table within a database.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return fileSymbols List of paths to table within database.
buildTablePaths:{[db:`s;tname:`s]
    if[0=count files:key db; :`$()];
    if[any files like "par.txt"; :raze .z.s[;tname] each hsym `$read0 .Q.dd[db;`par.txt]];
    files@:where files like "[0-9]*";
    $[count files; (.Q.dd[db;] ,[;tname]@) each files; enlist .Q.dd[db;tname]]
 };

// @brief Get the name of the domain used by the given column.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param cname symbol Column name.
// @return symbol Domain name.
colDomainName:{[db;tname;cname] $[20h=type vals:get mostRecentTdir[db;tname],cname;key vals;`]};

// @brief Get the type of each column in a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return dict Mapping of column name to type (as a short).
colTypes:{[db;tname] type each .Q.V mostRecentTdir[db;tname]};

// @brief Enumerate symbol values.
// @param db fileSymbol Path to database root.
// @param domain symbol Sym file (domain) name.
// @param vals any Values to enumerate (simply returned if not symbols).
enum:{[db:`s;domain:`s;vals] $[11h=abs type vals; .Q.dd[db;domain]?vals; vals]};

// @brief Get all column names from a splayed table.
// @param tdir fileSymbol Table directory.
// @return symbols Column names (empty if tdir does not exist).
getColNames:{[tdir:`s] $[count key .Q.dd[tdir;`.d]; get tdir,`.d; `$()]};

// @brief Check whether a given name is valid (adheres to proper naming rules).
// @param name symbol Name to check.
// @return bool 1b if valid, 0b otherwise.
isValidName:{[name:`s] (name=.Q.id name) and not name in .Q.res,key`.q};

// @brief Get the table directory from the most recent partition.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @return fileSymbol Table directory.
mostRecentTdir:{[db;tname] last asc allTablePaths[db;tname]};

// @brief Check whether a given name is valid (adheres to proper naming rules). Signal error if not.
// @param name symbol Name to validate.
validateName:{[name:`s] if[not isValidName name; '"Invalid name: ",string name]};
