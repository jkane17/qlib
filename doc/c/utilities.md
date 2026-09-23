# Utilities

This document describes the general-purpose helpers from [`util.h`](/src/c/cdk/util.h).

## `saturatedSub`

```c
QSize saturatedSub(QSize a, QSize b);
```

Compute `a - b`, returning `0` instead of wrapping around when `b` is greater than or equal to `a`.

Because `QSize` is unsigned, an ordinary `a - b` with `b > a` wraps around to a very large value (see [`QSize`](/doc/c/type.md#qsize)). `saturatedSub` is useful for tracking the space remaining in a buffer, where the amount used may exceed what was available.

**Parameters**

| Parameter | Description     |
| --------- | --------------- |
| `a`       | The minuend     |
| `b`       | The subtrahend  |

**Returns**

`a - b`, or `0` if `b >= a`.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QSize remaining = 10;

    printf("%" PRIu64 "\n", saturatedSub(remaining, 3));  // 7
    printf("%" PRIu64 "\n", saturatedSub(remaining, 10)); // 0
    printf("%" PRIu64 "\n", saturatedSub(remaining, 25)); // 0 (not a wrapped-around value)

    return 0;
}
```

Output:

```
7
0
0
```
