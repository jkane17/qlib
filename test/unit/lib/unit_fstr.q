
/
    File:
        unit_fstr.q
    
    Description:
        Unit tests for fstr.q
\

.pkg.load `fstr`unit;

strRep:{[]
    // Atoms
    .unit.assert.match["1b";     .fstr.strRep 1b];
    .unit.assert.match[string g; .fstr.strRep g:first 1?0Ng];
    .unit.assert.match["0x10";   .fstr.strRep 0x10];
    .unit.assert.match[1#"7";    .fstr.strRep 7h];
    .unit.assert.match[1#"7";    .fstr.strRep 7i];
    .unit.assert.match[1#"7";    .fstr.strRep 7j];
    .unit.assert.match["7.5";    .fstr.strRep 7.5e];
    .unit.assert.match["7.5";    .fstr.strRep 7.5f];
    .unit.assert.match[1#"h";    .fstr.strRep "h"];
    .unit.assert.match["`hi";    .fstr.strRep `hi];
    .unit.assert.match[string t; .fstr.strRep t:.z.p];
    .unit.assert.match[string t; .fstr.strRep t:`month$.z.p];
    .unit.assert.match[string t; .fstr.strRep t:`date$.z.p];
    .unit.assert.match[string t; .fstr.strRep t:`datetime$.z.p];
    .unit.assert.match[string t; .fstr.strRep t:`timespan$.z.p];
    .unit.assert.match[string t; .fstr.strRep t:`minute$.z.p];
    .unit.assert.match[string t; .fstr.strRep t:`second$.z.p];
    .unit.assert.match[string t; .fstr.strRep t:`time$.z.p];

    // Simple lists
    .unit.assert.match["10b";           .fstr.strRep 10b];
    .unit.assert.match[.Q.s1 g;         .fstr.strRep g:2?0Ng];
    .unit.assert.match["0x1010";        .fstr.strRep 0x1010];
    .unit.assert.match["7 8";           .fstr.strRep 7 8h];
    .unit.assert.match["7 8";           .fstr.strRep 7 8i];
    .unit.assert.match["7 8";           .fstr.strRep 7 8j];
    .unit.assert.match["7.5 7.6";       .fstr.strRep 7.5 7.6e];
    .unit.assert.match["7.5 7.6";       .fstr.strRep 7.5 7.6f];
    .unit.assert.match["hi";            .fstr.strRep "hi"];
    .unit.assert.match["`hi`ho";        .fstr.strRep `hi`ho];
    .unit.assert.match[.Q.s1 t;         .fstr.strRep t:2#.z.p];
    .unit.assert.match[" " sv string t; .fstr.strRep t:`month$2#.z.p];
    .unit.assert.match[.Q.s1 t;         .fstr.strRep t:`date$2#.z.p];
    .unit.assert.match[.Q.s1 t;         .fstr.strRep t:`datetime$2#.z.p];
    .unit.assert.match[.Q.s1 t;         .fstr.strRep t:`timespan$2#.z.p];
    .unit.assert.match[.Q.s1 t;         .fstr.strRep t:`minute$2#.z.p];
    .unit.assert.match[.Q.s1 t;         .fstr.strRep t:`second$2#.z.p];
    .unit.assert.match[.Q.s1 t;         .fstr.strRep t:`time$2#.z.p];

    // General lists
    .unit.assert.match["(H;`H;1)";        .fstr.strRep ("H";`H;1)];
    .unit.assert.match["(Hi;`H`i;1 2)";   .fstr.strRep ("Hi";`H`i;1 2)];
    .unit.assert.match["(Hi;(`H`i;1 2))"; .fstr.strRep ("Hi";(`H`i;1 2))];
 };

// Helper to check 3 different match scenarios for .fstr.fmt
.unit_fstr.matchFmt:{[p;v;s]
    .unit.assert.match["-> ",s;       .fstr.fmt["-> ",p;v]];       // End replacement
    .unit.assert.match[s," <-";       .fstr.fmt[p," <-";v]];       // Start replacement
    .unit.assert.match["-> ",s," <-"; .fstr.fmt["-> ",p," <-";v]]; // Middle replacement
 };

fmt:{[]
    f:.unit_fstr.matchFmt["{}";;];

    // Atoms - Single replacement
    f[1b;   "1b"];
    f[g;    string g:first 1?0Ng];
    f[0x10; "0x10"]
    f[7h;   "7"];
    f[7i;   "7"];
    f[7j;   "7"];
    f[7.5e; "7.5"];
    f[7.5f; "7.5"];
    f["h";  "h"];
    f[t;    string t:.z.p];
    f[t;    string t:`month$.z.p];
    f[t;    string t:`date$.z.p];
    f[t;    string t:`datetime$.z.p];
    f[t;    string t:`timespan$.z.p];
    f[t;    string t:`minute$.z.p];
    f[t;    string t:`second$.z.p];
    f[t;    string t:`time$.z.p];

    // Simple lists - Single replacement
    f[10b;      "10b"];
    f[g;        .Q.s1 g:2?0Ng];
    f[0x1010;   "0x1010"];
    f[7 8h;     "7 8"];
    f[7 8i;     "7 8"];
    f[7 8j;     "7 8"];
    f[7.5 7.6e; "7.5 7.6"];
    f[7.5 7.6f; "7.5 7.6"];
    f["hi";     "hi"];
    f[`hi`ho;   "`hi`ho"];
    f[t;        .Q.s1 t:2#.z.p];
    f[t;        " " sv string t:`month$2#.z.p];
    f[t;        .Q.s1 t:`date$2#.z.p];
    f[t;        .Q.s1 t:`datetime$2#.z.p];
    f[t;        .Q.s1 t:`timespan$2#.z.p];
    f[t;        .Q.s1 t:`minute$2#.z.p];
    f[t;        .Q.s1 t:`second$2#.z.p];
    f[t;        .Q.s1 t:`time$2#.z.p];

    // General lists - Single replacement
    f[("H";`H;1);        "(H;`H;1)"];
    f[("Hi";`H`i;1 2);   "(Hi;`H`i;1 2)"];
    f[("Hi";(`H`i;1 2)); "(Hi;(`H`i;1 2))"];

    f:{[v;s] .unit_fstr.matchFmt["{} {} {}";v;" " sv 3#enlist (),s]};

    // Atoms - Mulitple replacements
    f[1b;   "1b"];
    f[g;    string g:first 1?0Ng];
    f[0x10; "0x10"]
    f[7h;   "7"];
    f[7i;   "7"];
    f[7j;   "7"];
    f[7.5e; "7.5"];
    f[7.5f; "7.5"];
    f["h";  "h"];
    f[t;    string t:.z.p];
    f[t;    string t:`month$.z.p];
    f[t;    string t:`date$.z.p];
    f[t;    string t:`datetime$.z.p];
    f[t;    string t:`timespan$.z.p];
    f[t;    string t:`minute$.z.p];
    f[t;    string t:`second$.z.p];
    f[t;    string t:`time$.z.p];

    f:.unit_fstr.matchFmt["{} & {}";;];
    
    // Simple lists - Mulitple replacements
    f[10b;      "1b & 0b"];
    f[g;        " & " sv string g:2?0Ng];
    f[0x1010;   "0x10 & 0x10"];
    f[7 8h;     "7 & 8"];
    f[7 8i;     "7 & 8"];
    f[7 8j;     "7 & 8"];
    f[7.5 7.6e; "7.5 & 7.6"];
    f[7.5 7.6f; "7.5 & 7.6"];
    f["hi";     "h & i"];
    f[`hi`ho;   "`hi & `ho"];
    f[t;        " & " sv string t:2#.z.p];
    f[t;        " & " sv string t:`month$2#.z.p];
    f[t;        " & " sv string t:`date$2#.z.p];
    f[t;        " & " sv string t:`datetime$2#.z.p];
    f[t;        " & " sv string t:`timespan$2#.z.p];
    f[t;        " & " sv string t:`minute$2#.z.p];
    f[t;        " & " sv string t:`second$2#.z.p];
    f[t;        " & " sv string t:`time$2#.z.p];

    f:.unit_fstr.matchFmt["{} & {} & {}";;];

    // General lists - Mulitple replacements
    f[("H";`H;1);             "H & `H & 1"];
    f[("Hi";`H`i;1 2);        "Hi & `H`i & 1 2"];
    f[("Hi";(`H`i;1 2);7.5f); "Hi & (`H`i;1 2) & 7.5"];
    
    // Mismatch
    .unit.assert.fail[.fstr.fmt;("{} {} {}";`a`b);"mismatch"];
    .unit.assert.fail[.fstr.fmt;("{} {}";`a`b`c);"mismatch"];

    // Early exit
    .unit_fstr.matchFmt["{}";();"{}"];
    .unit_fstr.matchFmt["";1;""];
 };

.unit.add[`fstr;] each `strRep`fmt;
