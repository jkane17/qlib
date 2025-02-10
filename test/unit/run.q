
/
    File:
        run.q
    
    Description:
        Run unit tests.
\

.pkg.load `unit;

.unit.loadSuites .pkg.internal.path.unit;
results:.unit.run[];
.unit.printResults results;

exit 0;
