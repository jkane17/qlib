
/
    File        : run.q
    Description : Run unit tests.
\

.pkg.load `os`unit;

.unit.loadSuites .pkg.internal.path.unit;
results:.unit.run[];

line:"--------------------";
passed:select sum duration by suite from results[`passed] 
    where not suite in exec distinct suite from results`failed;
failed:2!select suite, case, errMsg, errStmt from results`failed;

-1 "\n",line;
-1 "PASSED";
-1 line,"\n";
-1 .Q.s2 passed;

-1 "\n",line;
-1 "FAILED";
-1 line,"\n";
if[count failed; -1 .Q.s2 failed];

exit 0;
