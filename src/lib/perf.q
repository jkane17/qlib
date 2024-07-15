
/
    Performance Testing
\

// @brief Time and space of a function application or string evaluation.
// @param x : (String || General List) : String to evaluate, or function 
// application where the first item in the list is the function name (Symbol)
// and susequent items are its arguments.
// @return General List : Fist items is time and space usage, second item is 
// the result of the evaluation/application.
.perf.ts:{.Q.ts . e[0],enlist 1_e:.util.ptree x};

// @brief Memory usage of a function application or string evaluation.
// @param x : (String || General List) : String to evaluate, or function 
// application where the first item in the list is the function name (Symbol)
// and susequent items are its arguments.
// @return Long : Space usage.
.perf.memUse:.[;0 1].perf.ts@;

// @brief Time statistics of n function applications or string evaluations.
// @param n : Long : Number of times to repeat application/evaluation.
// @param expr : (String || General List) : String to evaluate, or function 
// application where the first item in the list is the function name (Symbol)
// and susequent items are its arguments.
// @return Dict : Time statistics.
.perf.timeit:{[n;expr] 
    e:.util.ptree expr;
    s:.z.N; 
    do[n;value e]; 
    t:.z.N-s;
    $[10=type expr;[f:expr;args:1_e];[f:first expr;args:1_ expr]];
    `f`args`n`total`average!(f;args;n;t;"n"$t%n)
 };

// @brief Time statistics and memory usage of n function applications or string 
// evaluations.
// @param n : Long : Number of times to repeat application/evaluation.
// @param expr : (String || General List) : String to evaluate, or function 
// application where the first item in the list is the function name (Symbol)
// and susequent items are its arguments.
// @return Dict : Time statistics and memory usage.
.perf.test:{[n;expr] timeit[n;expr],(1#`mem)!1#memUse expr};
