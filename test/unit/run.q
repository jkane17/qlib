
/
    File:
        run.q
    
    Description:
        Run unit tests.
\

.pkg.load `os`unit;

print:{[msg;results]
    line:40#"-";
    -1 "\n",line;
    -1 msg,"\n";
    -1 .Q.s2 results;
    -1 "\n",line,"\n";
 };

.unit.loadSuites .pkg.internal.path.unit;
results:.unit.run[];

passed:select sum duration by suite from results[`passed] 
    where not suite in exec distinct suite from results`failed;
failed:2!select suite, case, errMsg, errStmt from results`failed;

print["PASSED";passed];
if[count failed; print["FAILED";failed]];

exit 0;
