# dbm.q

A modernised version of the original KX `dbmaint.q` script used for database maintenance.

## Contents

- [`addCol`](#addcol)
- [`addCols`](#addcols)
- [`addMissingCols`](#addmissingcols)
- [`addTab`](#addtab)
- [`castCol`](#castcol)
- [`copyCol`](#copycol)
- [`delCol`](#delcol)
- [`delTab`](#deltab)
- [`domainUnused`](#domainunused)
- [`domainUsage`](#domainusage)
- [`domainUsed`](#domainused)
- [`fnCol`](#fncol)
- [`hasCol`](#hascol)
- [`listCols`](#listcols)
- [`listTabs`](#listtabs)
- [`persistDomainMap`](#persistdomainmap)
- [`rebuildDomains`](#rebuilddomains)
- [`reenumerateAll`](#reenumerateall)
- [`reenumerateAllFrom`](#reenumerateallfrom)
- [`reenumerateCol`](#reenumeratecol)
- [`reenumerateColFrom`](#reenumeratecolfrom)
- [`reenumerateTab`](#reenumeratetab)
- [`reenumerateTabFrom`](#reenumeratetabfrom)
- [`renameCol`](#renamecol)
- [`renameDomain`](#renamedomain)
- [`renameTab`](#renametab)
- [`reorderCols`](#reordercols)
- [`resolveDomainMap`](#resolvedomainmap)
- [`rmAttr`](#rmattr)
- [`setAttr`](#setattr)
- [`strToSymCol`](#strtosymcol)
- [`symToStrCol`](#symtostrcol)

## `addCol`

```q
addCol[db;domain;tname;cname;default]
```

Add a column to a database table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `domain` | `symbol` | Sym file (domain) name (only used if column is symbol type). |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |
| `default` | `any` | Default value of the column. |

**Examples**

```q
// Add a newCol to the trade table with a default value of 10
dbm.addCol[`:db;`;`trade;`newCol;10]
```

```q
// Add a newSymCol to the trade table with a default value of `abc, enumeated against mySym
dbm.addCol[`:db;`mySym;`trade;`newSymCol;`abc]
```

## `addCols`

```q
addCols[db;domain;tname;cnames;default]
```

Add columns of matching types to a database table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `domain` | `symbol` | Sym file (domain) name (only used if columns are symbol type). |
| `tname` | `symbol` | Table name. |
| `cnames` | `symbol[]` | Column names. |
| `default` | `any` | Default value of the columns. |

**Examples**

```q
// Add newColA & newColB to the trade table with a default value of 10
dbm.addCols[`:db;`;`trade;`newColA`newColB;10]
```

```q
// Add newSymColA & newSymColB to the trade table with a default value of `abc, enumerated
// against mySym
dbm.addCols[`:db;`mySym;`trade;`newSymColA`newSymColB;`abc]
```

## `addMissingCols`

```q
addMissingCols[db;tname;goodTdir]
```

Add missing columns across all partitions of a table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `goodTdir` | `fileSymbol` | Path of a table directory which has no missing columns. |

**Example**

```q
// Add any missing columns to the trade table which exist in the 2025.12.17 partition, but
// are missing from other partitions
dbm.addMissingCols[`:db;`trade;`:db/2025.12.17/trade]
```

## `addTab`

```q
addTab[db;domain;tname;schema]
```

Add a new table to all partitions of a database.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `domain` | `symbol` | Sym file (domain) name. |
| `tname` | `symbol` | New table name. |
| `schema` | `table` | New table schema. |

**Example**

```q
// Add the quote table to the database
dbm.addTab[`:db;`sym;`quote;([] sym:`$(); ap:"f"$(); bp:"f"$())]
```

## `castCol`

```q
castCol[db;tname;cname;typ]
```

Cast a column to a given type.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |
| `typ` | `short\|char\|symbol` | Type to cast column to. |

**Example**

```q
// Cast the size column, in the trade table, to a float
dbm.castCol[`:db;`trade;`size;"f"]
```

## `copyCol`

```q
copyCol[db;tname;srcCol;dstCol]
```

Copy a column across all partitions of a table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `srcCol` | `symbol` | Column name whose data will be copied. |
| `dstCol` | `symbol` | New column name that will be created. |

**Example**

```q
// Copy the size column to a new column called sizeCopy in the trade table
dbm.copyCol[`:db;`trade;`size;`sizeCopy]
```

## `delCol`

```q
delCol[db;tname;cname]
```

Delete a column from a database table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |

**Example**

```q
// Delete the column oldCol from the trade table
dbm.delCol[`:db;`trade;`oldCol]
```

## `delTab`

```q
delTab[db;tname]
```

Delete a table from a database.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |

**Example**

```q
// Delete the trade table
dbm.delTab[`:db;`trade]
```

## `domainUnused`

```q
domainUnused[db;tname]
```

Build a mapping of domain name to indexes of unused symbols in the given database table.
If a domain is not used at all, it will not show up in the result.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |

**Returns** `dict` — Mapping of domain name to indexes of unused symbols.

**Example**

```q
// Unused symbol indexes per domain of the trade table
q)dbm.domainUnused[`:db;`trade]
sym     | 5 6 7
otherSym| 2 8
```

## `domainUsage`

```q
domainUsage db
domainUsage[db]
```

Build a mapping of domain name to indexes of used symbols in the given database.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |

**Returns** `dict` — Mapping of domain name to indexes of used symbols.

**Example**

```q
// Used symbol indexes per domain of the entire database
q)dbm.domainUsage `:db
sym     | 0 1 2 3 4
otherSym| 0 1 3 4 5 6 7 9
```

## `domainUsed`

```q
domainUsed[db;tname]
```

Build a mapping of domain name to indexes of used symbols in the given database table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |

**Returns** `dict` — Mapping of domain name to indexes of used symbols.

**Example**

```q
// Used symbol indexes per domain of the trade table
q)dbm.domainUsed[`:db;`trade]
sym     | 0 2 4
otherSym| 0 3 4 7 9
```

## `fnCol`

```q
fnCol[db;tname;cname;fn]
```

Apply a function to a column across all partitions of a table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |
| `fn` | `function` | Unary function to apply to the column. |

**Examples**

```q
// Multiply the price column in the trade table by 2
dbm.fnCol[`:db;`trade;`price;2*]
```

```q
// Make the characters in the alpha column of the trade table uppercase
dbm.fnCol[`:db;`trade;`alpha;upper]
```

## `hasCol`

```q
hasCol[db;tname;cname]
```

Does the given column exist in the table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |

**Returns** `boolean` — `1b` if the column exists within the table, `0b` otherwise.

**Example**

```q
// Does the trade table have the size column?
dbm.hasCol[`:db;`trade;`size]
```

## `listCols`

```q
listCols[db;tname]
```

List all column names of the given table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |

**Returns** `symbol[]` — Column names.

**Example**

```q
// List the columns of the trade table
q)dbm.listCols[`:db;`trade]
`time`sym`venue`size`price`company`moves
```

## `listTabs`

```q
listTabs db
listTabs[db]
```

List all table names in a database.

> [!NOTE]
> Uses the most recent partition in a partitioned database.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |

**Returns** `symbol[]` — Table names.

**Example**

```q
// List the tables of the given database
q)dbm.listTabs `:db
`trade`quote
```

## `persistDomainMap`

```q
persistDomainMap[dir;dm]
```

Save each domain as a file containing the symbol values.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `dir` | `fileSymbol` | Path to persist to. |
| `dm` | `dict` | Mapping of domain name to symbol values. |

**Returns** `dict` — Mapping of domain name to persisted path.

**Example**

```q
// Persist a domain map
q)dm
sym     | IBM AMZN GOOGL
otherSym| A D E H J

q)dbm.persistDomainMap[`:temp;dm]
sym     | :temp/sym
otherSym| :temp/otherSym
```

## `rebuildDomains`

```q
rebuildDomains[db;dir]
```

Recreate all domains of the given database with only the used symbols.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `dir` | `fileSymbol` | Path to persist to. |

**Returns** `dict` — Mapping of domain name to persisted path.

**Example**

```q
// Rebuild domains of the given database
q)dbm.rebuildDomains[`:db;`:temp]
sym     | :temp/sym
otherSym| :temp/otherSym
```

## `reenumerateAll`

```q
reenumerateAll[db;newDomainFile]
```

Re-enumerate every table in the database.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against. |

**Example**

```q
// Re-enumerate all tables within the given database against the temp/new domain file
dbm.reenumerateAll[`:db;`:temp/new]
```

## `reenumerateAllFrom`

```q
reenumerateAllFrom[db;currDomainName;newDomainFile]
```

Re-enumerate every table in the database from a given domain.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `currDomainName` | `symbol` | Name of the current domain. |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against. |

**Example**

```q
// Re-enumerate all columns within the given database, currently enumerated against sym,
// against the temp/new domain file
dbm.reenumerateAllFrom[`:db;`sym;`:temp/new]
```

## `reenumerateCol`

```q
reenumerateCol[db;tname;cname;newDomainFile]
```

Re-enumerate a column against a new domain across all partitions.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against. |

**Example**

```q
// Re-enumerate the venue column within the trade table against the temp/new domain file
dbm.reenumerateCol[`:db;`trade;`venue;`:temp/new]
```

## `reenumerateColFrom`

```q
reenumerateColFrom[db;tname;cname;currDomainName;newDomainFile]
```

Re-enumerate a column from a specific domain to a new domain.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |
| `currDomainName` | `symbol` | Name of the current domain. |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against. |

**Example**

```q
// Re-enumerate the venue column within the trade table against the temp/new domain file
// only if it is currently enumerated against sym
dbm.reenumerateColFrom[`:db;`trade;`venue;`sym;`:temp/new]
```

## `reenumerateTab`

```q
reenumerateTab[db;tname;newDomainFile]
```

Re-enumerate all enumeration columns in a table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against. |

**Example**

```q
// Re-enumerate all symbol columns within the trade table against the temp/new domain file
dbm.reenumerateTab[`:db;`trade;`:temp/new]
```

## `reenumerateTabFrom`

```q
reenumerateTabFrom[db;tname;currDomainName;newDomainFile]
```

Re-enumerate all enumeration columns in a table from a given domain.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `currDomainName` | `symbol` | Name of the current domain. |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against. |

**Example**

```q
// Re-enumerate all symbol columns within the trade table, currently enumerated against sym,
// against the temp/new domain file
dbm.reenumerateTabFrom[`:db;`trade;`sym;`:temp/new]
```

## `renameCol`

```q
renameCol[db;tname;old;new]
```

Rename a column across all partitions of a table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `old` | `symbol` | Current column name. |
| `new` | `symbol` | New column name. |

**Example**

```q
// Rename the size column to sizeRenamed in the trade table
dbm.renameCol[`:db;`trade;`size;`sizeRenamed]
```

## `renameDomain`

```q
renameDomain[db;currName;newName]
```

Rename a (symbol) domain.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `currName` | `symbol` | Current domain name. |
| `newName` | `symbol` | New domain name. |

**Example**

```q
// Rename the sym domain to symRenamed
dbm.renameDomain[`:db;`sym;`symRenamed]
```

## `renameTab`

```q
renameTab[db;old;new]
```

Rename a table in all partitions.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `old` | `symbol` | Current table name. |
| `new` | `symbol` | New table name. |

**Example**

```q
// Rename the trade table to quote
dbm.renameTab[`:db;`trade;`quote]
```

## `reorderCols`

```q
reorderCols[db;tname;order]
```

Reorder the columns across all partitions of a table.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `order` | `symbol[]` | New ordering of the columns (some or all). |

**Example**

```q
// Reorder the columns of the trade table so that sym, time, and price appear first
dbm.reorderCols[`:db;`trade;`sym`time`price]
```

## `resolveDomainMap`

```q
resolveDomainMap[db;dm]
```

Convert a domain mapping from index values to symbol values.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `dm` | `dict` | Mapping of domain name to indexes. |

**Returns** `dict` — Mapping of domain name to symbols.

**Example**

```q
// Resolve a domain map of indexes to symbols
q)dm
sym     | 0 2 4
otherSym| 0 3 4 7 9

q)dbm.resolveDomainMap[`:db;dm]
sym     | IBM AMZN GOOGL
otherSym| A D E H J
```

## `rmAttr`

```q
rmAttr[db;tname;cname]
```

Remove an attribute from a column.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |

**Example**

```q
// Remove attribute from the sym column in the trade table
dbm.rmAttr[`:db;`trade;`sym]
```

## `setAttr`

```q
setAttr[db;tname;cname;attrb]
```

Set an attribute on a column.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |
| `attrb` | `symbol` | Attribute (s, u, p, g). |

**Example**

```q
// Apply the parted attribute to the sym column in the trade table
dbm.setAttr[`:db;`trade;`sym;`p]
```

## `strToSymCol`

```q
strToSymCol[db;tname;cname;dname]
```

Convert a column from string type to symbol type.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |
| `dname` | `symbol` | Name of domain to enumerate against. |

**Example**

```q
// Convert the company column of the trade table from string to symbol type, enumerated
// against the sym domain
dbm.strToSymCol[`:db;`trade;`company;`sym]
```

## `symToStrCol`

```q
symToStrCol[db;tname;cname]
```

Convert a column from symbol type to string type.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `db` | `fileSymbol` | Path to database root. |
| `tname` | `symbol` | Table name. |
| `cname` | `symbol` | Column name. |

**Example**

```q
// Convert the venue column of the trade table from symbol to string type
dbm.symToStrCol[`:db;`trade;`venue]
```
