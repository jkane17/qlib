
/
    @file
        math.q
    
    @description
        Mathematical Utilities.
\

// @brief Compute the factorial of the given integer (n!).
// @param n long Integer to compute factorial for.
// @return long n!.
factorial:{[n:`j] prd 1+til n};

// @brief Compute the number of ordered k-element subsets (k-permutations) of n elements, i.e.,
// computes n! / (n - k)!
// @param n long The total number of elements in the set.
// @param k long The number of elements to select and arrange.
// @return long The number of ordered subsets (permutations) of size k from n elements.
permCount:{[n:`j;k:`j] prd (n-k-1)+til k};

// @brief Compute the number of unordered k-element subsets (k-combinations) of n elements, i.e., 
// computes n! / (k!(n - k)!)
// @param n long The total number of elements in the set.
// @param k long The number of elements to select.
// @return long The number of unordered subsets (combinations) of size k from n elements.
combCount:{[n:`j;k:`j] permCount[n;k] div factorial k};

// @brief Build the unordered k-element subsets (k-combinations) taken from n elements.
// @param n long The total number of elements in the set.
// @param k long The number of elements to select.
// @return list k-element subsets (index masks).
comb:{[n:`j;k:`j] 
    $[
        k=n; enlist til k; 
        k=1; enlist each til n; 
        .z.s[n-1;k],.z.s[n-1;k-1],\:enlist n-1
    ] 
 };

// @breif Splits n into k parts as evenly as possible, distributing any remainder from left to 
// right.
// @param n long The number to split.
// @param k long The number of parts to split into.
// @return longs k parts.
balancedPartition:{[n:`j;k:`j] p+c,(k-count c:(n-sum p:k#n div k)#1)#0};


export:([factorial; permCount; combCount; comb; balancedPartition]);
