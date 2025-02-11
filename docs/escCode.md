# escCode.q
Terminal escape code handling.
## .escCode.apply
Apply escape codes to the given text.
### Syntax
```q
.escCode.apply[codes;text]
```
### Parameters
|Parameter|Type|Description|
|-|-|-|
|codes|Symbols|Escape codes to apply.|
|text|String|Text to apply escape codes to.|
### Return
* **Type**: String
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
* **Type**: Symbols
* **Description**: Escape codes.
## .escCode.listFGColors
List foreground colors.
### Syntax
```q
.escCode.listFGColors[]
```
### Return
* **Type**: Symbols
* **Description**: Foreground colors.
## .escCode.listBGColors
List background colors.
### Syntax
```q
.escCode.listBGColors[]
```
### Return
* **Type**: Symbols
* **Description**: Background colors.
## .escCode.listColors
List all colors.
### Syntax
```q
.escCode.listColors[]
```
### Return
* **Type**: Symbols
* **Description**: All colors.
