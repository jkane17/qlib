
unit:use`qlib.unit;

unit.registerSrcDir .Q.dd[unit.currentDir[];`$"../src"];
unit.registerTestDir .Q.dd[unit.currentDir[];`unit];

if[not `in in key .Q.opt .z.X;
    results:unit.run[];
    unit.print.summary results;
    unit.print.failures results;
    exit 0;
 ];
