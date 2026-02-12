
/
    @file
        dbm.q
    
    @description
        Database Maintenance.
\

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

// @brief Add missing columns across all partitions of a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param goodTdir fileSymbol Path of a table directory which has no missing columns.
addMissingCols:{[db:`s;tname:`s;goodTdir:`s]
    add1MissingCols[;goodTdir] peach allTablePaths[db;tname] except goodTdir;
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
listCols:{[db:`s;tname:`s] getColNames last allTablePaths[db;tname]};

// @brief Rename a column across all partitions of a table.
// @param db fileSymbol Path to database root.
// @param tname symbol Table name.
// @param old symbol Current column name.
// @param new symbol New column name.
renameCol:{[db:`s;tname:`s;old:`s;new:`s] 
    validateName new;
    rename1Col[;old;new] peach allTablePaths[db;tname];
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

export:([
    addCol; addMissingCols; castCol; copyCol; delCol; delTab;
    fnCol; hasCol; listCols; renameCol; renameTab; reorderCols; 
    rmAttr; setAttr
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
// @param goodTdir fileSymbol Path of a table directory which has no missing columns.
add1MissingCols:{[tdir:`s;goodTdir:`s]
    goodCols:getColNames goodTdir;
    if[count missing:goodCols except getColNames tdir;
        {[d;g;c] 
            add1Col[d;c;] $[(t="*") or t within "AZ";enlist;] (t:"*"^meta[g][c;`t])$()
        }[tdir;goodTdir;] each missing;
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
        copy . .Q.dd[tdir;] each srcCol,dstCol;
        if[(hname:`$string[srcCol],"#") in key tdir; 
            copy . .Q.dd[tdir;] each hname,`$string[dstCol],"#"
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

// @brief Apply a function to a single database table.
// @param tdir fileSymbol Table directory.
// @param cname symbol Column name.
// @param fn function Unary function to apply to the column.
fn1Col:{[tdir:`s;cname:`s;fn]
    if[cname in getColNames tdir;
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

// @brief Rename a column in a single database table.
// @param tdir fileSymbol Table directory.
// @param old symbol Current column name.
// @param new symbol New column name.
rename1Col:{[tdir:`s;old:`s;new:`s]
    if[(old in colNames) and not new in colNames:getColNames tdir;
        rename . .Q.dd[tdir;] each old,new;
        if[(hname:`$string[old],"#") in key tdir; 
            rename . .Q.dd[tdir;] each hname,`$string[new],"#"
        ];
        @[tdir;`.d;:;.[colNames;where colNames=old;:;new]]
    ]
 };

// @brief Rename a table.
// @param old fileSymbol Path to current table within a partition.
// @param new fileSymbol Path to new table within a partition.
rename1Tab:{[old:`s;new:`s] if[()~key new; rename[old;new]]};

// @brief Reorder the columns in a single database table.
// @param tdir fileSymbol Table directory.
// @param order symbols New ordering of the columns.
reorder1Cols:{[tdir:`s;order:`S]
    if[not all exists:order in colNames:getColNames tdir;
        '"Unknown column(s): ","," sv string order where not exists
    ];
    @[tdir;`.d;:;order,colNames except order];
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
    if[any files like "par.txt"; :raze (.z.s[;tname] hsym@) each `$read0 .Q.dd[db;`par.txt]];
    files@:where files like "[0-9]*";
    $[count files; (.Q.dd[db;] ,[;tname]@) each files; enlist .Q.dd[db;tname]]
 };

// @brief Convert a file path to a correctly formatted OS string.
// @param path fileSymbol File path to convert.
// @return String Converted file path.
convertPath:{[path:`s]
    path:string path;
    if[isWindows; path[where"/"=path]:"\\"];
    (":"=first path)_ path
 };

// @brief Copy a source file to a destination file.
// @param src fileSymbol File to copy.
// @param dst fileSymbol Location to copy to.
copy:{[src:`s;dst:`s] system $[isWindows; "copy /v /z "; "cp "]," " sv convertPath each src,dst;};

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

isWindows:.z.o in`w32`w64;

// @brief Copy a source file to a destination file.
// @param src fileSymbol File to copy.
// @param dst fileSymbol Location to copy to.
rename:{[src:`s;dst:`s] system $[isWindows; "move "; "mv "]," " sv convertPath each src,dst;};

// @brief Check whether a given name is valid (adheres to proper naming rules). Signal error if not.
// @param name symbol Name to validate.
validateName:{[name:`s] if[not isValidName name; '"Invalid name: ",string name]};
