# math.q

Mathematical functions.

## Contents

- [`factorial`](#factorial)
- [`permCount`](#permcount)
- [`combCount`](#combcount)
- [`comb`](#comb)
- [`balancedPartition`](#balancedpartition)

## `factorial`

```q
factorial n
factorial[n]
```

Compute the factorial of the given integer, i.e., computes $n!$.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `n` | `long` | Integer to compute factorial for. |

**Returns** `long` — $n!$.

## `permCount`

```q
permCount[n;k]
```

Compute the number of ordered `k`-element subsets (`k`-permutations) of `n` elements, i.e.,
computes $\cfrac{n!}{(n - k)!}$

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `n` | `long` | The total number of elements in the set. |
| `k` | `long` | The number of elements to select and arrange. |

**Returns** `long` — The number of ordered subsets (permutations) of size `k` from `n` elements.

## `combCount`

```q
combCount[n;k]
```

Compute the number of unordered `k`-element subsets (`k`-combinations) of `n` elements, i.e.,
computes $\cfrac{n!}{k!(n - k)!}$

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `n` | `long` | The total number of elements in the set. |
| `k` | `long` | The number of elements to select. |

**Returns** `long` — The number of unordered subsets (combinations) of size `k` from `n` elements.

## `comb`

```q
comb[n;k]
```

Build the unordered `k`-element subsets (`k`-combinations) taken from `n` elements.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `n` | `long` | The total number of elements in the set. |
| `k` | `long` | The number of elements to select. |

**Returns** `list` — `k`-element subsets (index masks).

## `balancedPartition`

```q
balancedPartition[n;k]
```

Splits `n` into `k` parts as evenly as possible, distributing any remainder from left to
right.

**Parameters**

| Name | Type | Description |
| ---- | ---- | ----------- |
| `n` | `long` | The number to split. |
| `k` | `long` | The number of parts to split into. |

**Returns** `long[]` — `k` parts.
