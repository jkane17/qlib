
/ 
    Format String
\

// @brief Convert args into their string representation.
// @param args : Any : Values to be converted.
// @return String : String representation.
.fstr.strRep:{[args]
    ty:type args;
    $[
        ty=0h; "(",(";" sv .z.s each args),")";
        abs[ty] in 1 4 11h; .Q.s1 args;
        ty=10h; args;
        ty in 98 99 104h; .Q.s2 args;
        ty>0h; " " sv string args;
        string args
    ]
 };

// @brief Replace "{}" with positional values (similar to Python fstring).
// @param str  : String : Format string.
// @param args : Any    : Arguments to insert.
// @return String : Formated string.
.fstr.fmt:{[str;args] 
    i:ss[str;"{}"];
    ci:count i;
    ca:count args;
    if[all (ci<>ca;1<>ca;not (1=ci) or type[args] within 1 19h); '`mismatch];
    args,:();
    // Handle a simple list into one position
    args:$[(ci=1) and ca>1;enlist .fstr.strRep args;.fstr.strRep each args];
    (raze/) .[;(::;1);:;args] 2 cut (0,i) cut str
 };
