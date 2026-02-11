
unit:use`qlib.unit;

unit.registerSrcDir .Q.dd[unit.currentDir[];`$"../src"];
unit.registerTestDir .Q.dd[unit.currentDir[];`unit];

results:unit.run[];
unit.print.summary results;
unit.print.failures results;

exit 0;
