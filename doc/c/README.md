# C Interface

A C interface for creating, inspecting, and managing Q/kdb+ objects. It wraps the kdb+ C API (`k.h`) with descriptive names, C types that mirror Q's types, and additional checking.

## Usage

Include the single umbrella header:

```c
#include "q.h"
```

`q.h` includes all of the headers below. The headers can also be included individually.

**Requirements:**

- **C23** (`-std=c2x`), for enumerations with a fixed underlying type. Tested with GCC 13 and Clang 18. The headers can also be included from C++ (tested as C++20).
- **GCC or Clang**, as `QObj` relies on a compiler extension (see [Layout Compatibility](/doc/c/object.md#layout-compatibility)).
- Linking against KX's C API library (for example, `c.o` or `e.o` on Linux and macOS, or `c.dll` and `c.lib` on Windows), or loading into a kdb+ process as a shared library.

Debug builds check the arguments of the accessor functions with `assert`. Define `NDEBUG` (as `build.sh --release` does) to remove these checks.

## Documentation

| Document                            | Header    | Contents                                                        |
| ----------------------------------- | --------- | --------------------------------------------------------------- |
| [Q Types](type.md)                  | `type.h`  | C types for Q values, type codes, attributes, and special values |
| [Q Object](object.md)               | `obj.h`   | The `QObj` structure and its memory layout                      |
| [Creation](creating.md)             | `new.h`   | Creating atoms, lists, dictionaries, and tables                 |
| [Access](accessing.md)              | `get.h`   | Reading atom values, list items, and parts of tables            |
| [Type Checking](checking.md)        | `is.h`    | Checking what kind of value an object holds                     |
| [Errors](errors.md)                 | `err.h`   | Signalling and retrieving errors                                |
| [Memory](memory.md)                 | `mem.h`   | Reference counting and ownership                                |
| [Symbols](symbols.md)               | `sym.h`   | Interning strings as symbols                                    |
| [Formatting](formatting.md)         | `fmt.h`   | Writing Q values as strings and Q literals                      |
| [Utilities](utilities.md)           | `util.h`  | General-purpose helpers                                         |

`ipc.h` (connecting to and querying other Q processes over IPC) does not yet provide any functions: its declarations of the kdb+ IPC functions (`khp`, `k`, `kclose`, and others) are commented out.

## Example

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QSymbol names[] = {"sym", "price"};
    QSymbol syms[] = {"abc", "xyz"};
    QFloat prices[] = {1.5, 2.25};

    QObj *table =
        qNewTable(qNewSymbolList(names, 2), qNewSymbolList(syms, 2), qNewFloatList(prices, 2));
    if (!table) {
        printf("error: %s\n", qGetError(qCheckError(table)));
        return 1;
    }

    QObj *columns = qGetTableColumns(table);
    QObj *symColumn = qGetMixedAtIndex(columns, 0);
    QObj *priceColumn = qGetMixedAtIndex(columns, 1);

    for (QSize i = 0; i < qGetTableRowCount(table); i++)
        printf("%s %.2f\n", qGetSymbolAtIndex(symColumn, i), qGetFloatAtIndex(priceColumn, i));

    qDecRef(table);
    return 0;
}
```

Output:

```
abc 1.50
xyz 2.25
```
