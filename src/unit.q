
/
    @file
        unit.q
    
    @description
        Unit Testing Framework.

        Suite
            A collection of related test cases, typically defined within a single file.
            It groups together tests that target a common unit, module, or feature.

        Case
            A single test function that verifies one logical unit of behaviour.  
            A case may contain multiple assertions, but they all support testing the same 
            unit of functionality.
\

fs:use`qlib.fs;

///// PUBLIC /////

// @brief Get the current directory of the test module.
// @return fileSymbol Current directory.
currentDir:{[] first ` vs first ` vs .Q.rp`:::};

// @brief Get the name of the current test suite.
// @return symbol Current test suite name.
suiteName:{[] fs.basenameNoExt first ` vs .Q.rp`:::};

// @brief Create a temporary directory.
// @return fileSymbol Path to temporary directory.
createTmpDir:{[]
    suite:suiteName[];
    fs.mkdir dir:.Q.dd[fs.tmpdir[];(`qlibunit;`$raze string suite,"j"$.z.p)];
    tmpDirs[suite]:dir;
    dir
 };

// @brief Register the test directory where all unit test files can be found.
// @param dir fileSymbol Directory to register.
registerTestDir:{[dir]
    files:key dir;
    (dir;parent):` vs dir;
    .Q.m.SP,:1_string dir;
    addSuite[parent;] each files;
    .Q.m.SP:-1_.Q.m.SP;
 };

// @brief Register the source directory where all source files can be found (must be done before
// registering a test directory).
// @param dir fileSymbol Directory to register.
registerSrcDir:{[dir] srcDir::dir;};

// @brief Load a source file (from within the source dir if registered).
// @param file symbol File to load (path relative to source directory if registered).
loadSrc:{[file]
    system "l ",$[null srcDir; string file; 1_string .Q.dd[srcDir;file]];
 };

// @brief Run all tests.
run:{[] raze runSuite each 1_key suites};

// @brief Run all cases within the given suite.
// @param suite symbol Test suite to run.
// @return table Test results.
runSuite:{[suite] 
    results:runCase[suite;] each key suites suite;
    deleteTmpDir suite;
    results
 };

// @brief Run a single test case.
// @param suite symbol Suite that contains the test case.
// @param case symbol Test case to run.
// @return table Test results.
runCase:{[suite;case]
    caseFunc:suites[suite;case];
    res:([suite;case;start:.z.p]);
    res[`errInfo]:execute[suite;caseFunc];
    res,:([pass:(::)~res`errInfo; duration:.z.p-res`start]);
    resetMocks[];
    res
 };

// @brief Mock a value for the remainder of the test case (automatically restored).
// @param moduleName symbol The name of the test module (obtained via .z.M).
// @param name symbol Name of value you want to mock.
// @param newValue any The new value that the name should hold.
mock:{[moduleName;name;newValue]
    fname:` sv moduleName,name;
    if[(::)~mocks fname; mocks[fname]:get fname];
    fname set newValue;
 };

// @brief Assert x is a truthy value.
// @param x any Q object.
assert.true:{
    if[not x; '"ASSERT TRUE | Expected ",.Q.s1[x]," to be a truthy value"]
 };

// @brief Assert x is a falsey value.
// @param x any Q object.
assert.false:{
    if[x; '"ASSERT FALSE | Expected ",.Q.s1[x]," to be a falsey value"]
 };

// @brief Assert that x equals y.
// @param x any Q object.
// @param y any Q object.
assert.eq:{
    if[not x=y; '"ASSERT EQUAL | Expected ",.Q.s1[x]," = ",.Q.s1 y]
 };

// @brief Assert that x matches y.
// @param x any Q object.
// @param y any Q object.
assert.match:{
    if[not x~y; '"ASSERT MATCH | Expected ",.Q.s1[x]," ~ ",.Q.s1 y]
 };

// @brief Assert that x matches y.
// @param x any Q object.
// @param y any Q object.
assert.like:{
    if[not x like y; '"ASSERT LIKE | Expected ",.Q.s1[x]," like ",.Q.s1 y]
 };

// @brief Assert that function application raises an error.
// @param f function Function to apply.
// @param args list Function arguments.
// @param err string Expected error.
assert.fail:{[f;args;err]
    if[(0h>type args) or args~(); args:enlist args];
    .[
        f;
        args;   
        {if[not x like y; '"ASSERT FAIL | Expected error ",.Q.s1[x],", but got ",.Q.s1 y]}[err;]
    ]
 };

// @brief Print a summary of the results to standard output.
// @param results table Results table (returned from the `run` function).
print.summary:{[results]
    summary:`suite`case xasc select suite, case, duration, pass from results;
    stdout (""; "Unit Test Summary"),(indentSize#" "),/:(    
        "Duration   | ",string sum summary`duration; 
        "Test Count | ",string count summary; 
        "Passed     | ",string sum summary`pass; 
        "Failed     | ",string sum not summary`pass;
        ""
    ),.Q.s2 summary;
 };

// @brief Print failed test cases to standard output.
// @param results table Results table (returned from the `run` function).
print.failures:{[results]
    if[count failures:select from results where not pass;
        stdout (""; "Unit Test Failures"),(indentSize#" "),/:raze formatFailure each failures
    ]
 };

export:([
    createTmpDir; currentDir; suiteName; registerTestDir; registerSrcDir; 
    loadSrc; run; runSuite; runCase; mock; assert; print 
 ]);


///// PRIVATE /////

indentSize:4;
stdout:-1;

srcDir:`;

mocks:(1#`$())!1#(::);
suites:(1#`$())!1#(::);
tmpDirs:([]);

results:(
    [suite:`$(); case:`$()]
    pass:"b"$(); start:"p"$(); duration:"n"$()
 );

addSuite:{[parent;file] suites[name]:use ` sv parent,name:first ` vs file};

execute:{[suite;func]
    .Q.trp[
        func;
        ();
        {[suite;err;bt]
            bts:extractFromBacktrace each bt;
            i:where suite=(fs.basenameNoExt hsym `$) each bts[;`file];            
            ([err]),@[;`file;first] flip bts $[count i;i;0]
        }[suite;;]
    ]
 };

extractFromBacktrace:{[bt]
    (;file;lineStart;funcStr):bt 1;
    charIndex:bt 2;

    lineNum:lineStart+sum "\n"=charIndex#funcStr;

    split:` vs funcStr;
    charSums:sums 1+count each split;
    idx:(charIndex<charSums)?1b;
    lineStr:split idx;
    countBeforeTrim:count lineStr;
    lineStr:trim lineStr;

    lineIdx:0|(charIndex-charSums idx-1)-countBeforeTrim-count lineStr;

    ([file;lineNum;lineStr;lineIdx])
 };

resetMocks:{[] {key[x] set' value x} 1_mocks};

deleteTmpDir:{[suite]
    fs.rmrf each tmpDirs;
    tmpDirs::tmpDirs _ suite;
 };

formatFailure:{[res]
    (    
        "Suite | ",string res`suite; 
        "Case  | ",string res`case; 
        "File  | ",res[`errInfo;`file];
        "Error | ",res[`errInfo;`err]
    ),"        ",/:raze reverse formatErr ./: flip res[`errInfo;`lineNum`lineStr`lineIdx]
 };

formatErr:{[lineNum;lineStr;lineIdx]
    num:"[",string[lineNum],"] ";
    (num,lineStr; #[lineIdx+count num;" "],"^")
 };
