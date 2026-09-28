# unit.q

Unit testing framework.

Suite
A collection of related test cases, typically defined within a single file.
It groups together tests that target a common unit, module, or feature.

Case
A single test function that verifies one logical unit of behaviour.
A case may contain multiple assertions, but they all support testing the same
unit of functionality.

**Special export values**:
| Name | Description |
| ------ | ------------------------------------------------- |
| `init` | Always run before any test cases run in the suite |

## Contents

- [Public](#public)
    - [`currentDir`](#currentdir)
    - [`suiteName`](#suitename)
    - [`createTmpDir`](#createtmpdir)
    - [`registerTestDir`](#registertestdir)
    - [`run`](#run)
    - [`runSuite`](#runsuite)
    - [`runCase`](#runcase)
    - [`mock`](#mock)
    - [`assert.true`](#asserttrue)
    - [`assert.false`](#assertfalse)
    - [`assert.eq`](#asserteq)
    - [`assert.match`](#assertmatch)
    - [`assert.like`](#assertlike)
    - [`assert.fail`](#assertfail)
    - [`setColor`](#setcolor)
    - [`print.summary`](#printsummary)
    - [`print.failures`](#printfailures)

## Public

### `currentDir`

```q
currentDir[]
```

Get the current directory of the test module.

**Returns** `fileSymbol` — Current directory.

**Example**

```q
q)unit.currentDir[]
`:/home/user/qlib/test/q
```

### `suiteName`

```q
suiteName[]
```

Get the name of the current test suite.

**Returns** `symbol` — Current test suite name.

**Example**

```q
q)unit.suiteName[]
`test_clap
```

### `createTmpDir`

```q
createTmpDir[]
```

Create a temporary directory.

**Returns** `fileSymbol` — Path to temporary directory.

**Example**

```q
q)unit.createTmpDir[]
`:/tmp/qlib/unit/843739810360896131/run
```

### `registerTestDir`

```q
registerTestDir dir
registerTestDir[dir]
```

Register the test directory where all unit test files can be found.

**Parameters**

| Name  | Type         | Description            |
| ----- | ------------ | ---------------------- |
| `dir` | `fileSymbol` | Directory to register. |

**Example**

```q
unit.registerTestDir .Q.dd[unit.currentDir[];`unit]
```

### `run`

```q
run[]
```

Run all tests.

**Returns** `table` — Test results.

**Example**

```q
q)unit.run[]
suite     case         start                         errInfo pass duration
--------------------------------------------------------------------------------------
test_clap testRaw      2026.09.26D12:27:26.256728762 ::      1    0D00:00:00.000046610
test_clap testRegister 2026.09.26D12:27:26.256798277 ::      1    0D00:00:00.000103790
test_clap testParse    2026.09.26D12:27:26.256910504 ::      1    0D00:00:00.000384504
..
```

### `runSuite`

```q
runSuite suite
runSuite[suite]
```

Run all cases within the given suite.

**Parameters**

| Name    | Type     | Description        |
| ------- | -------- | ------------------ |
| `suite` | `symbol` | Test suite to run. |

**Returns** `table` — Test results.

**Example**

```q
q)unit.runSuite `test_clap
suite     case            start                         errInfo pass duration
-----------------------------------------------------------------------------------------
test_clap testRaw         2026.09.26D12:34:15.371053924 ::      1    0D00:00:00.000058313
test_clap testRegister    2026.09.26D12:34:15.371133478 ::      1    0D00:00:00.000065858
test_clap testParse       2026.09.26D12:34:15.371205828 ::      1    0D00:00:00.000259773
test_clap testParseErrors 2026.09.26D12:34:15.371472945 ::      1    0D00:00:00.000187383
```

### `runCase`

```q
runCase[suite;case]
```

Run a single test case.

**Parameters**

| Name    | Type     | Description                        |
| ------- | -------- | ---------------------------------- |
| `suite` | `symbol` | Suite that contains the test case. |
| `case`  | `symbol` | Test case to run.                  |

**Returns** `table` — Test results.

**Example**

```q
q)unit.runCase[`test_clap;`testRaw]
suite   | `test_clap
case    | `testRaw
start   | 2026.09.26D12:37:05.084200873
errInfo | ::
pass    | 1b
duration| 0D00:00:00.000043284
```

### `mock`

```q
mock[module;name;mockValue]
```

Mock a value for the remainder of the test case (automatically restored).

**Parameters**

| Name        | Type     | Description                                                         |
| ----------- | -------- | ------------------------------------------------------------------- |
| `module`    | `symbol` | The full name of the module where the name to be mocked is defined. |
| `name`      | `symbol` | Name of value you want to mock.                                     |
| `mockValue` | `any`    | The value that the name should hold.                                |

**Example**

```q
mock[`qlib.clap;`raw;{[] `blah}]
```

### `assert.true`

```q
assert.true x
assert.true[x]
```

Assert `x` is a truthy value.

**Parameters**

| Name | Type  | Description |
| ---- | ----- | ----------- |
| `x`  | `any` | Q object.   |

**Example**

```q
unit.assert.true 1b
```

### `assert.false`

```q
assert.false x
assert.false[x]
```

Assert `x` is a falsey value.

**Parameters**

| Name | Type  | Description |
| ---- | ----- | ----------- |
| `x`  | `any` | Q object.   |

**Example**

```q
unit.assert.false 0b
```

### `assert.eq`

```q
assert.eq[x;y]
```

Assert that `x` equals `y`.

**Parameters**

| Name | Type  | Description |
| ---- | ----- | ----------- |
| `x`  | `any` | Q object.   |
| `y`  | `any` | Q object.   |

**Example**

```q
unit.assert.equal[8;9]
```

### `assert.match`

```q
assert.match[x;y]
```

Assert that `x` matches `y`.

**Parameters**

| Name | Type  | Description |
| ---- | ----- | ----------- |
| `x`  | `any` | Q object.   |
| `y`  | `any` | Q object.   |

**Example**

```q
unit.assert.matches[8;9]
```

### `assert.like`

```q
assert.like[x;y]
```

Assert that `x` like `y`.

**Parameters**

| Name | Type  | Description |
| ---- | ----- | ----------- |
| `x`  | `any` | Q object.   |
| `y`  | `any` | Q object.   |

**Example**

```q
unit.assert.like["hello";"goodbye"]
```

### `assert.fail`

```q
assert.fail[f;args;err]
```

Assert that function application raises an error.

**Parameters**

| Name   | Type       | Description         |
| ------ | ---------- | ------------------- |
| `f`    | `function` | Function to apply.  |
| `args` | `list`     | Function arguments. |
| `err`  | `string`   | Expected error.     |

**Examples**

```q
unit.assert.fail[clap.parse;();"Missing argument(s): requiredArg"
```

```q
unit.assert.fail[dbm.strToSymCol; (db;`trade;`size;`sym); "type"]
```

### `setColor`

```q
setColor enable
setColor[enable]
```

Enable or disable colour (ANSI escape codes) in printed output (disabled by default).

**Parameters**

| Name     | Type      | Description             |
| -------- | --------- | ----------------------- |
| `enable` | `boolean` | Whether to use colour.  |

**Example**

```q
unit.setColor 1b
```

### `print.summary`

```q
print.summary results
print.summary[results]
```

Print a summary of the results to standard output.

**Parameters**

| Name      | Type    | Description                                       |
| --------- | ------- | ------------------------------------------------- |
| `results` | `table` | Results table (returned from the `run` function). |

**Example**

```q
unit.print.summary unit.run[]
```

### `print.failures`

```q
print.failures results
print.failures[results]
```

Print failed test cases to standard output.

**Parameters**

| Name      | Type    | Description                                       |
| --------- | ------- | ------------------------------------------------- |
| `results` | `table` | Results table (returned from the `run` function). |

**Example**

```q
unit.print.failures unit.run[]
```
