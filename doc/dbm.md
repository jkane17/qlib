# dbm - Database Maintenance Utilities

This script is a modernised version of the original KX dbmaint.q script used for database maintenance.

## Using

Copy [dbm.q](../src/dbm.q) into a directory on your module search path, for example:

```
/home/user/.kx/mod/qlib/dbm.q
```

Then import it:

```q
dbm:use`qlib.dbm
```

## Functions

### `addCol`

Add a column to a database table.

```q
dbm.addCol[db;domain;tname;cname;default]
```

#### Parameters

| Param     | Type         | Description                                                 |
| --------- | ------------ | ----------------------------------------------------------- |
| `db`      | `fileSymbol` | Path to database root                                       |
| `domain`  | `symbol`     | Sym file (domain) name (only used if column is symbol type) |
| `tname`   | `symbol`     | Table name                                                  |
| `cname`   | `symbol`     | Column name                                                 |
| `default` | `any`        | Default value of the column                                 |

#### Examples

Add a new column (`newCol`) to the `trade` table with a default value of 10:

```q
dbm.addCol[`:db;`;`trade;`newCol;10]
```

Add a new column (`newSymCol`) to the `trade` table, within all partitions under `db`, with a default value of `` `abc``, enumeated against `mySym`:

```q
dbm.addCol[`:db;`mySym;`trade;`newSymCol;`abc]
```

### `addCols`

Add columns of matching types to a database table.

```q
dbm.addCols[db;domain;tname;cnames;default]
```

#### Parameters

| Param     | Type         | Description                                                   |
| --------- | ------------ | ------------------------------------------------------------- |
| `db`      | `fileSymbol` | Path to database root                                         |
| `domain`  | `symbol`     | Sym file (domain) name (only used if columns are symbol type) |
| `tname`   | `symbol`     | Table name                                                    |
| `cnames`  | `symbols`    | Column names                                                  |
| `default` | `any`        | Default value of the columns                                  |

#### Examples

Add new columns (`newColA` & `newColB`) to the `trade` table with a default value of 10:

```q
dbm.addCol[`:db;`;`trade;`newColA`newColB;10]
```

Add new columns (`newSymColA` & `newSymColB`) to the `trade` table, within all partitions under `db`, with a default value of `` `abc``, enumerated against `mySym`:

```q
dbm.addCols[`:db;`mySym;`trade;`newSymColA`newSymColB;`abc]
```

### `addMissingCols`

Add missing columns across all partitions of a table.

```q
dbm.addMissingCols[db;tname;goodTdir]
```

#### Parameters

| Param      | Type         | Description                                            |
| ---------- | ------------ | ------------------------------------------------------ |
| `db`       | `fileSymbol` | Path to database root                                  |
| `tname`    | `symbol`     | Table name                                             |
| `goodTdir` | `fileSymbol` | Path of a table directory which has no missing columns |

#### Example

Add any missing columns, to the `trade` table, which exist in the `2025.12.17` partition, but are missing from other partitions.

```q
dbm.addMissingCols[`:db;`trade;`:db/2025.12.17/trade]
```

### `addTab`

Add a new table to all partitions of a database.

```q
dbm.addTab[db;domain;tname;schema]
```

#### Parameters

| Param    | Type         | Description            |
| -------- | ------------ | ---------------------- |
| `db`     | `fileSymbol` | Path to database root  |
| `domain` | `symbol`     | Sym file (domain) name |
| `tname`  | `symbol`     | Table name             |
| `schema` | `table`      | New table schema       |

#### Example

Add the `quote` table to the database.

```q
dbm.addTab[`:db;`quote;([] sym:`$(); ap:"f"$(); bp:"f"$())]
```

### `castCol`

Cast a column to a given type.

```q
dbm.castCol[db;tname;cname;typ]
```

#### Parameters

| Param     | Type                  | Description            |
| --------- | --------------------- | ---------------------- |
| `db`      | `fileSymbol`          | Path to database root  |
| `tname`   | `symbol`              | Table name             |
| `cname`   | `symbol`              | Column name            |
| `default` | `short\|char\|symbol` | Type to cast column to |

#### Example

Cast the `size` column, in the `trade` table, to a `float`:

```q
dbm.castCol[`:db;`trade;`size;"f"]
```

### `copyCol`

Copy a column across all partitions of a table.

```q
dbm.copyCol[db;tname;srcCol;dstCol]
```

#### Parameters

| Param    | Type         | Description                           |
| -------- | ------------ | ------------------------------------- |
| `db`     | `fileSymbol` | Path to database root                 |
| `tname`  | `symbol`     | Table name                            |
| `srcCol` | `symbol`     | Column name whose data will be copied |
| `dstCol` | `symbol`     | New column name that will be created  |

#### Example

Copy the `size` column to a new column called `sizeCopy` in the `trade` table:

```q
dbm.copyCol[`:db;`trade;`size;`sizeCopy]
```

### `delCol`

Delete a column from a database table.

```q
dbm.delCol[db;tname;cname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |
| `cname` | `symbol`     | Column name           |

#### Example

Delete the column `oldCol` from the `trade` table.

```q
dbm.delCol[`:db;`trade;`oldCol]
```

### `delTab`

Delete a table from a database.

```q
dbm.delTab[db;tname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |

#### Example

Delete the `trade` table.

```q
dbm.delTab[`:db;`trade]
```

### `domainUnused`

Build a mapping of domain name to indexes of un-used symbols in the given database table. If a domain is not used at all, it will not show up in the result.

```q
dbm.domainUnused[db;tname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |

#### Returns

(`dict`) Mapping of domain name to indexes of unused symbols.

#### Example

Unused symbol indexes per domain of the `trade` table.

```q
q)dbm.domainUnused[`:db;`trade]
sym     | 5 6 7
otherSym| 2 8
```

### `domainUsage`

Build a mapping of domain name to indexes of used symbols in the given database.

```q
dbm.domainUsage db
```

#### Parameters

| Param | Type         | Description           |
| ----- | ------------ | --------------------- |
| `db`  | `fileSymbol` | Path to database root |

#### Returns

(`dict`) Mapping of domain name to indexes of used symbols.

#### Example

Used symbol indexes per domain of the entire database.

```q
q)dbm.domainUsage `:db
sym     | 0 1 2 3 4
otherSym| 0 1 3 4 5 6 7 9
```

### `domainUsed`

Build a mapping of domain name to indexes of used symbols in the given database table.

```q
dbm.domainUsed[db;tname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |

#### Returns

(`dict`) Mapping of domain name to indexes of used symbols.

#### Example

Used symbol indexes per domain of the `trade` table.

```q
q)dbm.domainUsed[`:db;`trade]
sym     | 0 2 4
otherSym| 0 3 4 7 9
```

### `fnCol`

Apply a function to a column across all partitions of a table.

```q
dbm.fnCol[db;tname;cname;fn]
```

#### Parameters

| Param   | Type         | Description                           |
| ------- | ------------ | ------------------------------------- |
| `db`    | `fileSymbol` | Path to database root                 |
| `tname` | `symbol`     | Table name                            |
| `cname` | `symbol`     | Column name                           |
| `fn`    | `function`   | Unary function to apply to the column |

#### Examples

Multiply the `price` column in the `trade` table by 2:

```q
dbm.fnCol[`:db;`trade;`price;2*]
```

Make the characters in the `alpha` column of the `trade` table uppercase:

```q
dbm.fnCol[`:db;`trade;`alpha;upper]
```

### `hasCol`

Does the given column exist in the table.

```q
dbm.hasCol[db;tname;cname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |
| `cname` | `symbol`     | Column name           |

#### Returns

(`bool`) `1b` if the column exists within the table, `0b` otherwise.

#### Example

Does the `trade` table have the `size` column?

```q
dbm.hasCol[`:db;`trade;`size]
```

### `listCols`

List all column names of the given table.

```q
dbm.listCols[db;tname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |

#### Returns

(`symbols`) Column names.

#### Example

List the columns of the `trade` table.

```q
q)dbm.listCols[`:db;`trade]
`time`sym`venue`size`price`company`moves
```

### `listEnumCols`

List all enumeration (symbol) column names of the given table:

```q
dbm.listEnumCols[db;tname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |

#### Returns

(`symbols`) Enumeration (symbol) column names

#### Example

List the enumeration (symbol) columns of the `trade` table:

```q
q)dbm.listEnumCols[`:db;`trade]
`sym`venue
```

### `listTabs`

List all table names in a database.

> ![NOTE]
> Uses the most recent partition in a partitioned database.

```q
dbm.listTabs db
```

#### Parameters

| Param | Type         | Description           |
| ----- | ------------ | --------------------- |
| `db`  | `fileSymbol` | Path to database root |

#### Returns

(`symbols`) Table names.

#### Example

List the tables of the given database:

```q
q)dbm.listTabs `:db
`trade`quote
```

### `persistDomainMap`

Save each domain as a file containing the symbol values.

```q
dbm.persistDomainMap[dir;dm]
```

#### Parameters

| Param | Type         | Description                             |
| ----- | ------------ | --------------------------------------- |
| `dir` | `fileSymbol` | Path to persist to                      |
| `dm`  | `dict`       | Mapping of domain name to symbol values |

#### Returns

(`dict`) Mapping of domain name to persisted path.

#### Example

Persist a domain map:

```q
q)dm
sym     | IBM AMZN GOOGL
otherSym| A D E H J

q)dbm.persistDomainMap[`:temp;dm]
sym     | :temp/sym
otherSym| :temp/otherSym
```

### `rebuildDomains`

Recreate all domains of the given database with only the used symbols.

```q
dbm.rebuildDomains[db;dir]
```

#### Parameters

| Param | Type         | Description           |
| ----- | ------------ | --------------------- |
| `db`  | `fileSymbol` | Path to database root |
| `dir` | `fileSymbol` | Path to persist to    |

#### Returns

(`dict`) Mapping of domain name to persisted path.

#### Example

Rebuild domains of the given daabase:

```q
q)dbm.rebuildDomains[`:db;`:temp]
sym     | :temp/sym
otherSym| :temp/otherSym
```

### `reenumerateAll`

Re-enumerate every table in the database.

```q
dbm.reenumerateAll[db;newDomainFile]
```

#### Parameters

| Param           | Type         | Description                             |
| --------------- | ------------ | --------------------------------------- |
| `db`            | `fileSymbol` | Path to database root                   |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against |

#### Example

Re-enumerate all tables within the given database against the `temp/new` domain file:

```q
dbm.reenumerateAll[`:db;`:temp/new]
```

### `reenumerateAllFrom`

Re-enumerate every table in the database from a given domain.

```q
dbm.reenumerateAllFrom[db;currDomainName;newDomainFile]
```

#### Parameters

| Param            | Type         | Description                             |
| ---------------- | ------------ | --------------------------------------- |
| `db`             | `fileSymbol` | Path to database root                   |
| `currDomainName` | `symbol`     | Name of the current domain              |
| `newDomainFile`  | `fileSymbol` | New domain file to re-enumerate against |

#### Example

Re-enumerate all columns within the given database, currently enumerated against `sym`, against the `temp/new` domain file:

```q
dbm.reenumerateAllFrom[`:db;`sym;`:temp/new]
```

### `reenumerateCol`

Re-enumerate a column against a new domain across all partitions.

```q
dbm.reenumerateCol[db;tname;cname;newDomainFile]
```

#### Parameters

| Param           | Type         | Description                             |
| --------------- | ------------ | --------------------------------------- |
| `db`            | `fileSymbol` | Path to database root                   |
| `tname`         | `symbol`     | Table name                              |
| `cname`         | `symbol`     | Column name                             |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against |

#### Example

Re-enumerate the `venue` column within the `trade` table against the `temp/new` domain file:

```q
dbm.reenumerateCol[`:db;`trade;`venue;`:temp/new]
```

### `reenumerateColFrom`

Re-enumerate a column from a specific domain to a new domain.

```q
dbm.reenumerateColFrom[db;tname;cname;currDomainName;newDomainFile]
```

#### Parameters

| Param            | Type         | Description                             |
| ---------------- | ------------ | --------------------------------------- |
| `db`             | `fileSymbol` | Path to database root                   |
| `tname`          | `symbol`     | Table name                              |
| `cname`          | `symbol`     | Column name                             |
| `currDomainName` | `symbol`     | Name of the current domain              |
| `newDomainFile`  | `fileSymbol` | New domain file to re-enumerate against |

#### Example

Re-enumerate the `venue` column within the `trade` table against the `temp/new` domain file only if it is currently enumerated against `sym`:

```q
dbm.reenumerateColFrom[`:db;`trade;`venue;`sym;`:temp/new]
```

### `reenumerateTab`

Re-enumerate all enumeration columns in a table.

```q
dbm.reenumerateTab[db;tname;newDomainFile]
```

#### Parameters

| Param           | Type         | Description                             |
| --------------- | ------------ | --------------------------------------- |
| `db`            | `fileSymbol` | Path to database root                   |
| `tname`         | `symbol`     | Table name                              |
| `newDomainFile` | `fileSymbol` | New domain file to re-enumerate against |

#### Example

Re-enumerate all symbol columns within the `trade` table against the `temp/new` domain file:

```q
dbm.reenumerateTab[`:db;`trade;`:temp/new]
```

### `reenumerateTabFrom`

Re-enumerate all enumeration columns in a table from a given domain.

```q
dbm.reenumerateTabFrom[db;tname;currDomainName;newDomainFile]
```

#### Parameters

| Param            | Type         | Description                             |
| ---------------- | ------------ | --------------------------------------- |
| `db`             | `fileSymbol` | Path to database root                   |
| `tname`          | `symbol`     | Table name                              |
| `currDomainName` | `symbol`     | Name of the current domain              |
| `newDomainFile`  | `fileSymbol` | New domain file to re-enumerate against |

#### Example

Re-enumerate all symbol columns within the `trade` table, currently enumerated against `sym`, against the `temp/new` domain file:

```q
dbm.reenumerateTabFrom[`:db;`trade;`sym;`:temp/new]
```

### `renameCol`

Rename a column across all partitions of a table.

```q
dbm.renameCol[db;tname;old;new]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |
| `old`   | `symbol`     | Current column name   |
| `new`   | `symbol`     | New column name       |

#### Example

Rename the `size` column to `sizeRenamed` in the `trade` table:

```q
dbm.renameCol[`:db;`trade;`size;`sizeRenamed]
```

### `renameDomain`

Rename a (symbol) domain.

```q
dbm.renameDomain[db;currName;newName]
```

#### Parameters

| Param      | Type         | Description           |
| ---------- | ------------ | --------------------- |
| `db`       | `fileSymbol` | Path to database root |
| `currName` | `symbol`     | Current domain name   |
| `newName`  | `symbol`     | New domain name       |

#### Example

Rename the `sym` domain to `symRenamed`:

```q
dbm.renameDomain[`:db;`sym;`symRenamed]
```

### `renameTab`

Rename a table in all partitions.

```q
dbm.renameTab[db;old;new]
```

#### Parameters

| Param | Type         | Description           |
| ----- | ------------ | --------------------- |
| `db`  | `fileSymbol` | Path to database root |
| `old` | `symbol`     | Current table name    |
| `new` | `symbol`     | New table name        |

#### Example

Rename the `trade` table to `quote`:

```q
dbm.renameTab[`:db;`trade;`quote]
```

### `reorderCols`

Reorder the columns across all partitions of a table.

```q
dbm.reorderCols[db;tname;order]
```

#### Parameters

| Param   | Type         | Description                               |
| ------- | ------------ | ----------------------------------------- |
| `db`    | `fileSymbol` | Path to database root                     |
| `tname` | `symbol`     | Table name                                |
| `order` | `symbols`    | New ordering of the columns (some or all) |

#### Example

Reorder the columns of the `trade` table so that `sym`, `time`, and `price` appear first:

```q
dbm.reorderCols[`:db;`trade;`sym`time`price]
```

### `resolveDomainMap`

Convert a domain mapping from index values to symbol values.

```q
dbm.resolveDomainMap[db;dm]
```

#### Parameters

| Param | Type         | Description                        |
| ----- | ------------ | ---------------------------------- |
| `db`  | `fileSymbol` | Path to database root              |
| `dm`  | `dict`       | Mapping of domain name to indexes. |

#### Returns

(`dict`) Mapping of domain name to symbols.

#### Example

Reorder the columns of the `trade` table so that `sym`, `time`, and `price` appear first:

```q
q)dm
sym     | 0 2 4
otherSym| 0 3 4 7 9

q)dbm.resolveDomainMap[`:db;dm]
sym     | IBM AMZN GOOGL
otherSym| A D E H J
```

### `rmAttr`

Remove an attribute from a column.

```q
dbm.rmAttr[db;tname;cname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |
| `cname` | `symbol`     | Column name           |

#### Example

Remove attribute from the `sym` column in the `trade` table:

```q
dbm.rmAttr[`:db;`trade;`sym]
```

### `setAttr`

Set an attribute on a column.

```q
dbm.setAttr[db;tname;cname;attrb]
```

#### Parameters

| Param   | Type         | Description            |
| ------- | ------------ | ---------------------- |
| `db`    | `fileSymbol` | Path to database root  |
| `tname` | `symbol`     | Table name             |
| `cname` | `symbol`     | Column name            |
| `attrb` | `symbol`     | Attribute (s, u, p, g) |

#### Example

Apply the parted attribute to the `sym` column in the `trade` table:

```q
dbm.setAttr[`:db;`trade;`sym;`p]
```

### `strToSymCol`

Convert a column from string type to symbol type.

```q
dbm.strToSymCol[db;tname;cname;dname]
```

#### Parameters

| Param   | Type         | Description                         |
| ------- | ------------ | ----------------------------------- |
| `db`    | `fileSymbol` | Path to database root               |
| `tname` | `symbol`     | Table name                          |
| `cname` | `symbol`     | Column name                         |
| `dname` | `symbol`     | Name of domain to enumerate against |

#### Example

Convert the `company` column of the `trade` table from string to symbol type, enumerated against the `sym` domain:

```q
dbm.strToSymCol[`:db;`trade;`company;`sym]
```

### `symToStrCol`

Convert a column from symbol type to string type.

```q
dbm.symToStrCol[db;tname;cname]
```

#### Parameters

| Param   | Type         | Description           |
| ------- | ------------ | --------------------- |
| `db`    | `fileSymbol` | Path to database root |
| `tname` | `symbol`     | Table name            |
| `cname` | `symbol`     | Column name           |

#### Example

Convert the `venue` column of the `trade` table from symbol to string type:

```q
dbm.symToStrCol[`:db;`trade;`venue]
```
