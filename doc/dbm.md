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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `domain` | `symbol` | Sym file (domain) name (only used if column is symbol type) |
| `tname` | `symbol` | Table name |
| `cname` | `symbol` | Column name |
| `default` | `any` | Default value of the column |

#### Examples

Add a new column (`newCol`) to the `trade` table with a default value of 10:

```q
dbm.addCol[`:db;`;`trade;`newCol;10]
```

Add a new column (`newSymCol`) to the `trade` table, within all partitions under `db`, with a default value of `` `abc``, enumeated against `mySym`:

```q
dbm.addCol[`:db;`mySym;`trade;`newSymCol;`abc]
```

### `addMissingCols`

Add missing columns across all partitions of a table.

```q
dbm.addMissingCols[db;tname;goodTdir]
```

#### Parameters

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `domain` | `symbol` | Sym file (domain) name |
| `tname` | `symbol` | Table name |
| `schema` | `table` | New table schema |

#### Example

Add the `quote` table to the database.  

```q
dbm.addTab[`:db;`quote;([] sym:`$(); ap:"f"$(); bp:"f"$()]     
```

### `castCol`

Cast a column to a given type.

```q
dbm.castCol[db;tname;cname;typ]
```

#### Parameters

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `cname` | `symbol` | Column name |
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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `srcCol` | `symbol` | Column name whose data will be copied |
| `dstCol` | `symbol` | New column name that will be created |

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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `cname` | `symbol` | Column name |

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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |

#### Example

Delete the `trade` table.

```q
dbm.delTab[`:db;`trade]
```

### `fnCol`

Apply a function to a column across all partitions of a table.

```q
dbm.fnCol[db;tname;cname;fn]
```

#### Parameters

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `cname` | `symbol` | Column name |
| `fn` | `function` | Unary function to apply to the column |

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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `cname` | `symbol` | Column name |

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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |

#### Returns

(`symbols`) Column names.

#### Example

List the columns of the `trade` table.

```q
q)dbm.listCols[`:db;`trade]
`time`sym`size`price`company`moves
```

### `renameCol`

Rename a column across all partitions of a table.

```q
dbm.renameCol[db;tname;old;new] 
```

#### Parameters

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `old` | `symbol` | Current column name |
| `new` | `symbol` | New column name |

#### Example

Rename the `size` column to `sizeRenamed` in the `trade` table:

```q
dbm.renameCol[`:db;`trade;`size;`sizeRenamed]
```

### `renameTab`

Rename a table in all partitions.

```q
dbm.renameTab[db;old;new] 
```

#### Parameters

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `old` | `symbol` | Current table name |
| `new` | `symbol` | New table name |

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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `order` | `symbols` | New ordering of the columns (some or all) |

#### Example

Reorder the columns of the `trade` table so that `sym`, `time`, and `price` appear first:

```q
dbm.reorderCols[`:db;`trade;`sym`time`price]
```

### `rmAttr`

Remove an attribute from a column.

```q
dbm.rmAttr[db;tname;cname] 
```

#### Parameters

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `cname` | `symbol` | Column name |

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

| Param | Type | Description |
| - | - | - |
| `db` | `fileSymbol` | Path to database root |
| `tname` | `symbol` | Table name |
| `cname` | `symbol` | Column name |
| `attrb` | `symbol` | Attribute (s, u, p, g) |

#### Example

Apply the parted attribute to the `sym` column in the `trade` table:

```q
dbm.setAttr[`:db;`trade;`sym;`p]
```
