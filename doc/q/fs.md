# fs.q

Functions for performing common file system operations.

## Contents

- [`basename`](#basename)
- [`basenameNoExt`](#basenamenoext)
- [`copy`](#copy)
- [`dirname`](#dirname)
- [`exists`](#exists)
- [`isdir`](#isdir)
- [`isfile`](#isfile)
- [`mkdir`](#mkdir)
- [`move`](#move)
- [`rcopy`](#rcopy)
- [`listd`](#listd)
- [`listf`](#listf)
- [`rlist`](#rlist)
- [`rlistf`](#rlistf)
- [`rmrf`](#rmrf)
- [`tmpdir`](#tmpdir)
- [`toPlatformPath`](#toplatformpath)

## `basename`

```q
basename path
basename[path]
```

Get the basename of the given path.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | Path to get basename of. |

**Returns** `symbol` — Basename.

## `basenameNoExt`

```q
basenameNoExt path
basenameNoExt[path]
```

Get the basename of the given path without the file extension.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | Path to get basename of. |

**Returns** `symbol` — Basename without the file extension.

## `copy`

```q
copy[src;dst]
```

Copy a source file to a destination file.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `src` | `fileSymbol` | File to copy. |
| `dst` | `fileSymbol` | Location to copy to. |

## `dirname`

```q
dirname path
dirname[path]
```

Get the directory of the given path.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | Path to get directory name of. |

**Returns** `fileSymbol` — Directory name.

## `exists`

```q
exists path
exists[path]
```

Check if a given file/directory exists.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | Path of a file/directory. |

**Returns** `boolean` — `1b` if the path exists, `0b` otherwise.

## `isdir`

```q
isdir path
isdir[path]
```

Check if a given path is a directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | Path to check. |

**Returns** `boolean` — `1b` if path is a directory, `0b` otherwise.

## `isfile`

```q
isfile path
isfile[path]
```

Check if a given path is a file.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | Path to check. |

**Returns** `boolean` — `1b` if path is a file, `0b` otherwise.

## `mkdir`

```q
mkdir dir
mkdir[dir]
```

Create a directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `dir` | `fileSymbol` | Directory to create. |

## `move`

```q
move[src;dst]
```

Move/rename a source file to a destination file.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `src` | `fileSymbol` | File to move/rename. |
| `dst` | `fileSymbol` | Location to move/rename to. |

## `rcopy`

```q
rcopy[src;dst]
```

Recursively copy a source directory to a destination directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `src` | `fileSymbol` | File to copy. |
| `dst` | `fileSymbol` | Location to copy to. |

## `listd`

```q
listd dir
listd[dir]
```

List all top level sub-directories within a given directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `dir` | `fileSymbol` | Directory whose sub-directories are to be listed. |

**Returns** `fileSymbol[]` — Relative list of paths to sub-directories.

## `listf`

```q
listf dir
listf[dir]
```

List all top level files within a given directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `dir` | `fileSymbol` | Directory whose files are to be listed. |

**Returns** `fileSymbol[]` — Relative list of paths to files.

## `rlist`

```q
rlist dir
rlist[dir]
```

Recursively list all files and sub-directories within a given directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `dir` | `fileSymbol` | Top level directory whose contents are to be listed. |

**Returns** `fileSymbol[]` — Relative list of paths to files and sub-directories.

## `rlistf`

```q
rlistf dir
rlistf[dir]
```

Recursively list all files within a given directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `dir` | `fileSymbol` | Top level directory whose contents are to be listed. |

**Returns** `fileSymbol[]` — Relative list of paths to files.

## `rmrf`

```q
rmrf path
rmrf[path]
```

Foreceful and recursive removal of a file/directory.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | Path to file/directory to remove. |

## `tmpdir`

```q
tmpdir[]
```

Get the path to the default temporary directory.

**Returns** `fileSymbol` — Path to temporary directory.

## `toPlatformPath`

```q
toPlatformPath path
toPlatformPath[path]
```

Convert a file path to a correctly formatted string path based on the platform.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `path` | `fileSymbol` | File path to format. |

**Returns** `string` — Converted file path.
