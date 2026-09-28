# clap.q

A lightweight Q/KDB-X module for parsing command-line arguments in a structured and
type-safe way.

Arguments must first be registered so they can be validated, defaulted, and accessed in a
consistent format.

Parsing can then occur at any point before the arguments are used.

## Contents

- [`get`](#get)
- [`given`](#given)
- [`register`](#register)
- [`raw`](#raw)
- [`parse`](#parse)
- [`usage`](#usage)

## `get`

```q
get name
get[name]
```

Retrieve the value of an argument.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `name` | `symbol` | Argument name. |

**Returns** `any` — The parsed value of the argument, or `()` if it does not exist.

**Example**

```q
q)clap.get `myArg
10
```

## `given`

```q
given name
given[name]
```

Check whether an argument was supplied on the command line..

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `name` | `symbol` | Argument name. |

**Returns** `boolean` — `1b` if the argument was supplied, `0b` otherwise.

**Example**

```q
q)clap.given `myArg
1b
```

## `register`

```q
register[name;default;required;validate;description]
```

Register a command-line argument for parsing.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `name` | `symbol` | Argument name. |
| `default` | `any` | Default value. This also defines the argument’s type. Even if an argument is required, provide a null of the correct type. |
| `required` | `boolean` | Whether the argument is mandatory. |
| `validate` | `function` | A validation function applied to the parsed value. It must return a boolean. Use (::) if no validation is needed. |
| `description` | `string` | Human-readable description of the argument. |

**Example**

```q
q)clap.register[`myLongArg; 10; 0b; 0<=; "Must be ≥ 0"]
q)clap.register[`myStringArg; enlist "Nothing"; 0b; (::); "String argument"]
q)clap.register[`myRequiredArg; 0nf; 1b; (::); "Required float argument"]
```

## `raw`

```q
raw[]
```

Return raw command-line arguments before any parsing or type conversion.

**Returns** `dict` — Name Raw string value.

**Example**

```q
q)clap.raw[]
p     | "5000"
myArg1| "10"
myArg2| "hello"
```

## `parse`

```q
parse[]
```

Parse command line arguments.

**Examples**

```q
q)clap.parse[]  // Should return no error if successful
```

```q
q)clap.parse[]  // Invalid argument
'Invalid argument(s): myLongArg
  [0]  clap.parse[]
```

```q
q)clap.parse[]  // Missing required argument
'Missing argument(s): myRequiredArg
  [0]  clap.parse[]
       ^
```

## `usage`

```q
usage[]
```

Generate a usage/help table showing all registered arguments.

**Returns** `string` — A formatted table describing arguments, defaults, and requirements.

**Example**

```q
q)-1 clap.usage[]
name          default    required description
-----------------------------------------------------------------------
myLongArg     10         0        "My long argument which must be >= 0"
myStringArg   ,"Nothing" 0        "My string argument"
myRequiredArg 0n         1        "My required argument"
```
