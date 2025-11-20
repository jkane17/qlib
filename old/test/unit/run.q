
/
    @file
        run.q
    
    @description
        Run unit tests.
\

.qlib.module.load `unit;

.unit.loadSuites .pkg.internal.path.unit;
results:.unit.run[];
.unit.printResults results;

exit 0;
