# cast.q
Type casting utilities.
## .cast.htostr
Cast a file symbol to a string (removes prevaling ":").
### Syntax
```q
.cast.htostr h
```
### Parameters
|Parameter|Type|Description|
|-|-|-|
|h|FileSymbol|File symbol to cast.|
### Return
* **Type**: String
* **Description**: File symbol as a string.
### Examples
```q
.cast.htostr `:hello/world // -> "hello/world"
```
## .cast.tostr
Cast a value to a string.
### Syntax
```q
.cast.tostr x
```
### Parameters
|Parameter|Type|Description|
|-|-|-|
|x|Any|Value to cast.|
### Return
* **Type**: String
* **Description**: Value as a string.
### Examples
```q
.cast.tostr `hello  // -> "hello"
```
```q
.cast.tostr "hello" // -> "hello"
```
## .cast.tosym
Cast a value to a symbol.
### Syntax
```q
.cast.tosym x
```
### Parameters
|Parameter|Type|Description|
|-|-|-|
|x|Any|Value to cast.|
### Return
* **Type**: Symbol
* **Description**: Value as a symbol.
### Examples
```q
.cast.tosym `hello  // -> `hello
```
```q
.cast.tosym "hello" // -> `hello
```
## .cast.tohsym
Cast a value to a file symbol.
### Syntax
```q
.cast.tohsym x
```
### Parameters
|Parameter|Type|Description|
|-|-|-|
|x|Any|Value to cast.|
### Return
* **Type**: FileSymbol
* **Description**: Value as a file symbol.
### Examples
```q
.cast.tohsym "hello/world" // -> `:hello/world
```
