
\c 2000 2000

// Exit codes (checked by build.sh):
//     0 - Tests ran and all passed
//     2 - Tests could not be run
//     3 - Tests ran, but at least one failed

opts:.Q.def[([modulePath:enlist ""]);] .Q.opt .z.X;
if[count first opts`modulePath; .Q.m.SP:opts`modulePath];

main:{[opts]
    unit::use`qlib.unit;    // Global, so that it is available in interactive mode
    unit.setColor `color in key opts;
    unit.registerTestDir .Q.dd[unit.currentDir[];`unit];

    $[`itest in key opts;
        -1 "Running on port ",string system "p";
        [
            results::unit.run[];
            unit.print.summary results;
            unit.print.failures results;
            exit 3*not all results`pass
        ]
    ];
 };

.Q.trp[main;opts;{[err;bt] -2 "Error: ",err,"\n",.Q.sbt bt; exit 2}];
