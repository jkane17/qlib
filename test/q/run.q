
\c 2000 2000

opts:.Q.def[([modulePath:enlist ""]);] .Q.opt .z.X;
if[count first opts`modulePath; .Q.m.SP:opts`modulePath];

unit:use`qlib.unit;
unit.registerTestDir .Q.dd[unit.currentDir[];`unit];

$[`itest in key opts;
    -1 "Running on port ",string system "p";
    [
        results:unit.run[];
        unit.print.summary results;
        unit.print.failures results;
        exit not all results`pass    // 0 exit code if all passed, 1 otherwise
    ]
 ];
