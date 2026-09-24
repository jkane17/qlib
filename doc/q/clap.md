# clap - Command-Line Argument Parser

clap is a lightweight Q/KDB-X module for parsing command-line arguments in a structured and type-safe way.

Arguments must first be registered so they can be validated, defaulted, and accessed in a consistent format.

Parsing can then occur at any point before the arguments are used.

## Using

Copy [clap.q](../src/clap.q) into a directory on your module search path, for example:

```
/home/user/.kx/mod/qlib/clap.q
```

Then import it:

```q
clap:use`qlib.clap
```

## Functions

### `get`

Retrieve the value of an argument.

```q
clap.get name
```

#### Parameter

| Param | Type | Description |
| - | - | - |
| `name` | `Symbol` | Argument name |

#### Returns

(`any`) The parsed value of the argument, or `()` if it does not exist.

#### Example

```q
q)clap.get `myArg
10
```

### `given`

Check whether an argument was supplied on the command line.

```q
clap.given name
```

#### Parameter

| Param | Type | Description |
| - | - | - |
| `name` | `symbol` | Argument name |

#### Returns

(`bool`) `1b` if the argument was supplied, `0b` otherwise.

#### Example

```q
q)clap.given `myArg
1b
```

### `register`

Register a command-line argument for parsing.

```q
clap.register[name; default; required; validate; description]
```

#### Parameters

| Param | Type | Description |
| - | - | - |
| `name` | `symbol` | Argument name |
| `default` | `any` | Default value. This also defines the argument’s type. Even if an argument is required, provide a null of the correct type. |
| `required` | `bool` | Whether the argument is mandatory. |
| `validate` | `function` | A validation function applied to the parsed value. It must return a boolean. Use `(::)` if no validation is needed. |
| `description` | `string` | Human-readable description of the argument. |

#### Examples

```q
q)clap.register[`myLongArg; 10; 0b; 0<=; "Must be ≥ 0"]
q)clap.register[`myStringArg; enlist "Nothing"; 0b; (::); "String argument"]
q)clap.register[`myRequiredArg; 0nf; 1b; (::); "Required float argument"]
```

### `raw`

Return raw command-line arguments before any parsing or type conversion.

```q
clap.raw[]
```

#### Returns

(`dict`) Name → raw string value.

#### Example

```q
q)clap.raw[]
p     | "5000" 
myArg1| "10"   
myArg2| "hello"
```

### `parse`

Parse command line arguments.

```q
clap.parse[]
```

Errors occur when:
- An argument fails validation.
- A required argument is missing.

#### Examples

```q
q)clap.parse[]  // Should return no error if successful
```

Invalid argument:

```q
q)clap.parse[]
'Invalid argument(s): myLongArg
  [0]  clap.parse[]
       ^
```

Missing required argument:

```q
q)clap.parse[]
'Missing argument(s): myRequiredArg
  [0]  clap.parse[]
       ^   
```

### `usage`

Generate a usage/help table showing all registered arguments.

```q
clap.usage[]
```

#### Returns

(`string`) A formatted table describing arguments, defaults, and requirements.

#### Example

```q
q)-1 clap.usage[]
name          default    required description                                                   
------------------------------------------------------------------------------------------------
myLongArg     10         0        "My long argument which must be greater than or equal to zero"
myStringArg   ,"Nothing" 0        "My string argument"                                          
myRequiredArg 0n         1        "My required argument"  
```

A common pattern is to print usage information when parsing fails:

```q
if[@[{[] clap.parse[]; 0b}; (); {-2 "clap parsing error - ",x; 1b}];
    -2 clap.usage[];
    exit 1
 ]
```
