
/
    File        : unit.q
    Description : Unit testing framework.
\

.pkg.load `fstr`log;

.unit.internal.dataPath:.Q.dd[.pkg.internal.path.qlib;`test`data];

.unit.priv.tests:(
    [suite:"s"$();case:"s"$()]
    pass:"b"$(); start:"p"$(); duration:"n"$(); func:(); errMsg:(); errStmt:()
 );

.unit.priv.suitePattern:"unit_*.q";

.unit.priv.exec:{x[];(1b;"";())};

.unit.priv.findStmtEnd:{
    first i where (i:where x=";") in where 0=sum each {x+:y="[({"; x-:y="])}"; x}\[0 0 0;x]
 };

.unit.priv.extractErrStmt:{[bt]
    bt@:-1+?[;1b] bt[;1;0]~\:".unit.priv.exec";
    s:bt 2;
    b:s _ last bt 1;
    e:.unit.priv.findStmtEnd b;
    e#b
 };

.unit.priv.execCase:{[cf] 
    `pass`errMsg`errStmt!.Q.trp[.unit.priv.exec;cf;{(0b;x;.unit.priv.extractErrStmt y)}]
 };

.unit.priv.run:{[suite;case]
    r:.unit.priv.tests suite,case;
    r[`suite]:suite;
    r[`case]:case;
    r[`start]:.z.p;
    res:.unit.priv.execCase r`func;
    r[`pass]:res`pass;
    r[`errMsg]:res`errMsg;
    r[`errStmt]:res`errStmt;
    r[`duration]:.z.p-r`start;
    `.unit.priv.tests upsert r; 
    r`pass
 };

// Load a suite from the given file
.unit.loadSuite:{[file]
    .log.debug .fstr.fmt["Loading suite: {}";file];
    system "l ",1_string file;
 };

// Load all suite under the given directory
.unit.loadSuites:{[dir]
    .log.debug .fstr.fmt["Loading all suite within: {}";dir];
    p:.os.rls dir;
    f:(last ` vs) each p;
    suites:p where f like .unit.priv.suitePattern;
    .unit.loadSuite each suites;
 };

// Add a test to be run. Test should take no arguments
.unit.add:{[suite;case] 
    `.unit.priv.tests upsert enlist `suite`case`func!(suite;case;value case); 
 };

// Run a case
.unit.runCase:{[suite;case]
    .log.debug .fstr.fmt["Running case: {}";case];
    .unit.priv.run[suite;case]
 };

// Run all cases for the given suite
.unit.runCases:{[suite]
    all .unit.runCase[suite;] each cases:exec case from .unit.priv.tests where suite=suite
 };

// Run given suite
.unit.runSuite:{[suite] 
    .log.debug .fstr.fmt["Running suite: {}";suite];
    .unit.runCases suite
 };

// Run all suites
.unit.runSuites:{[] all .unit.runSuite each exec distinct suite from .unit.priv.tests};

// Main driver function
.unit.run:{[]
    pass:.unit.runSuites[];

    results:`passed`failed`summary`pass!(3#()),0b;
    results[`passed]:select suite, case, duration from .unit.priv.tests where pass;
    results[`failed]:select 
        suite, case, duration, errMsg, errStmt from .unit.priv.tests where not pass;
    results[`summary]:`passed`failed`total`duration!(
        count results`passed;
        count results`failed;
        count .unit.priv.tests;
        exec sum duration from .unit.priv.tests
    );
    results[`pass]:pass;

    results
 };

// Assert x is empty (0 = count)
.unit.assert.empty:{if[count x; '"ASSERT EMPTY - Expected input to be empty"]};

// Apply f to args and assert that the given error is raised
.unit.assert.fail:{[f;args;err]
    if[not .[f;args;~[err;]]; 
        '"ASSERT FAIL - Expected function application to fail with error: ",err
    ]
 };

// Assert that x is false
.unit.assert.false:{if[x; '"ASSERT FALSE - Expected input to be false"]};

// Assert that y matches the expected value x (x ~ y)
.unit.assert.match:{if[not x~y; '.fstr.fmt["ASSERT MATCH - Expected = {} : Actual = {}";(x;y)]]};

// Assert x is not empty (0 < count)
.unit.assert.notEmpty:{if[0=count x; '"ASSERT NOT EMPTY - Expected that input is not empty"]};

// Assert that x is true
.unit.assert.true:{if[not x; '"ASSERT TRUE - Expected input to be true"]};

// Assert that the type of y is equal to the expected type x
.unit.assert.type:{
    if[not x=type y; '.fstr.fmt["ASSERT TYPE - Expected = {} : Actual = {}";(x;type y)]]
 };
