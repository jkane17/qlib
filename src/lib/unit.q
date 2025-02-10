
/
    File:
        unit.q
    
    Description:
        Unit testing framework.
\

.pkg.load `fstr`log`os;

.unit.internal.dataPath:.Q.dd[.pkg.internal.path.qlib;`test`data];

.unit.priv.tests:(
    [suite:"s"$();case:"s"$()]
    pass:"b"$(); start:"p"$(); duration:"n"$(); func:(); errMsg:(); errStmt:()
 );

.unit.priv.suitePattern:"unit_*.q";

// @brief Execute a unit test.
// @param x Symbol Unit test function name.
// @return GeneralList 3 item list (pass;errMsg;errStmt) if test succeeds or error if tests fails.
.unit.priv.exec:{x[];(1b;"";())};

// @brief Find the end index of an error statement.
// @param x String Function definition.
// @return Long End index.
.unit.priv.findStmtEnd:{
    first i where (i:where x=";") in where 0=sum each {x+:y="[({"; x-:y="])}"; x}\[0 0 0;x]
 };

// @breif Extract an error statement from the given backtrace.
// @param bt GeneralList Backtrace object.
// @return String Error statement.
.unit.priv.extractErrStmt:{[bt]
    bt@:-1+?[;1b] bt[;1;0]~\:".unit.priv.exec";
    s:bt 2;
    b:s _ last bt 1;
    e:.unit.priv.findStmtEnd b;
    e#b
 };

// @brief Execute a unit case.
// @param cf Symbol Case function name.
// @return Dict Test result plus error information if applicable.
.unit.priv.execCase:{[cf] 
    `pass`errMsg`errStmt!.Q.trp[.unit.priv.exec;cf;{(0b;x;.unit.priv.extractErrStmt y)}]
 };

// @brief Run a case.
// @param suite Symbol Suite name.
// @param case Symbol Case name.
// @return Boolean Test result.
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

// @brief Print the results of the unit test.
// @param msg String Message to print above the results.
// @param results Dict Results.
.unit.priv.print:{[msg;results]
    line:40#"-";
    -1 "\n",line;
    -1 msg,"\n";
    -1 .Q.s2 results;
    -1 "\n",line,"\n";
 };

// @brief Load a suite from the given file.
// @param FileSymbol Suite file.
.unit.loadSuite:{[file]
    .log.debug .fstr.fmt["Loading suite: {}";file];
    system "l ",1_string file;
 };

// @brief Load all suites under the given directory.
// @param FileSymbol Directory.
.unit.loadSuites:{[dir]
    .log.debug .fstr.fmt["Loading all suite within: {}";dir];
    p:.os.rls dir;
    f:(last ` vs) each p;
    suites:p where f like .unit.priv.suitePattern;
    .unit.loadSuite each suites;
 };

// @brief Add a test to be run. Test function should take no arguments.
// @param suite Symbol Suite name.
// @param case Symbol Case name.
.unit.add:{[suite;case] `.unit.priv.tests upsert enlist `suite`case`func!(suite;case;value case);};

// @brief Run a case.
// @param suite Symbol Suite name.
// @param case Symbol Case name.
// @return Boolean Test result.
.unit.runCase:{[suite;case]
    .log.debug .fstr.fmt["Running case: {}";case];
    .unit.priv.run[suite;case]
 };

// @brief Run all cases for the given suite.
// @param suite Symbol Suite name.
// @return Boolean Test result.
.unit.runCases:{[suite]
    all .unit.runCase[suite;] each cases:exec case from .unit.priv.tests where suite=suite
 };

// @brief Run the given suite.
// @param suite Symbol Suite name.
// @return Boolean Test result.
.unit.runSuite:{[suite] 
    .log.debug .fstr.fmt["Running suite: {}";suite];
    .unit.runCases suite
 };

// @brief Run all suites.
// @return Boolean Test result.
.unit.runSuites:{[] all .unit.runSuite each exec distinct suite from .unit.priv.tests};

// @brief Main driver function
// @return Dict Results.
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

// @brief Print the results of the unit test.
// @param results Dict Results.
.unit.printResults:{[results]
    .unit.priv.print["PASSED";] select sum duration by suite from results[`passed] 
        where not suite in exec distinct suite from results`failed;
    if[count failed:2!select suite, case, errMsg, errStmt from results`failed;
        .unit.priv.print["FAILED";failed]
    ];
 };

// @breif Assert empty (0 = count).
// @param x Any Q object.
.unit.assert.empty:{if[count x; '"ASSERT EMPTY - Expected input to be empty"]};

// @brief Assert that function application raises an error.
// @param f Function Function to apply.
// @param args List Function arguments.
// @param err String Expected error.
.unit.assert.fail:{[f;args;err]
    if[not .[f;args;~[err;]]; 
        '"ASSERT FAIL - Expected function application to fail with error: ",err
    ]
 };

// @brief Assert false.
// @param x Any Q object.
.unit.assert.false:{if[x; '"ASSERT FALSE - Expected input to be false"]};

// @brief Assert in (x in y).
// @param x Any Q object to look for.
// @param y List List to check.
.unit.assert.in:{if[not x in y; '.fstr.fmt["ASSERT IN: Expected {} in {}";(x;y)]]};

// @brief Assert like (x like y).
// @param x String|Symbol String to check.
// @param y String Pattern to match.
.unit.assert.like:{if[not x like y; '.fstr.fmt["ASSERT LIKE - Expected {} like {}";(x;y)]]};

// @brief Assert match (x ~ y).
// @param x Any Q object.
// @param y Any Q object.
.unit.assert.match:{if[not x~y; '.fstr.fmt["ASSERT MATCH - Expected = {} : Actual = {}";(x;y)]]};

// @breif Assert not empty (0 < count).
// @param x Any Q object.
.unit.assert.notEmpty:{if[0=count x; '"ASSERT NOT EMPTY - Expected that input is not empty"]};

// @brief Assert true.
// @param x Any Q object.
.unit.assert.true:{if[not x; '"ASSERT TRUE - Expected input to be true"]};

// @brief Assert type.
// @param x Short Type.
// @param y Any Q object.
.unit.assert.type:{
    if[not x=type y; '.fstr.fmt["ASSERT TYPE - Expected = {} : Actual = {}";(x;type y)]]
 };
