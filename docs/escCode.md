# escCode.q
Terminal escape code handling.
## .escCode.apply
Apply escape codes to the given text.
### Syntax
```q
.escCode.apply[codes;text]
```
### Parameters
|Parameter|Type(s)|Description|
|-|-|-|
|codes|Symbols|Escape codes to apply.|
|text|String|Text to apply escape codes to.|
### Return
* **Type(s)**: String
* **Description**: Text wrapped with escape codes.
### Examples
```q
.escCode.apply[`BOLD`FG_RED;"Hello World"]
```
## .escCode.listAll
List all supported escape codes.
### Syntax
```q
.escCode.listAll[]
```
### Return
* **Type(s)**: Symbols
* **Description**: Escape codes.
## .escCode.listFGColors
List foreground colors.
### Syntax
```q
.escCode.listFGColors[]
```
### Return
* **Type(s)**: Symbols
* **Description**: Foreground colors.
## .escCode.listBGColors
List background colors.
### Syntax
```q
.escCode.listBGColors[]
```
### Return
* **Type(s)**: Symbols
* **Description**: Background colors.
## .escCode.listColors
List all colors.
### Syntax
```q
.escCode.listColors[]
```
### Return
* **Type(s)**: Symbols
* **Description**: All colors.
