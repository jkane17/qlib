
/
    File:
        fstr.q
    
    Description:
        Format strings.
\

// @brief Convert args into their string representation.
// @param args Any Values to be converted.
// @return String String representation.
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
// @param str String Format string.
// @param args Any Arguments to insert.
// @return String Formated string.
.fstr.fmt:{[str;args] 
    i:ss[str;"{}"];
    ci:count i;
    ca:count args;
    // Exit early no replacement to be performed
    if[any 0=ci,ca; :str];
    if[not any (ci=ca;1=ca;1=ci); '`mismatch];
    args,:();
    // Handle a simple list into one replacement position
    args:$[(ci=1) and ca>1;enlist .fstr.strRep args;.fstr.strRep each args];
    // Handle an atom into multiple replacement positions
    if[(ca=1) and ci>1; args:ci#args];
    first[str],raze args,'2_/:1_str:(0,i) cut str
 };
