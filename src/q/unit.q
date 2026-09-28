
/
    @file unit.q
    
    @overview
        Unit testing framework.

        Suite
            A collection of related test cases, typically defined within a single file.
            It groups together tests that target a common unit, module, or feature.

        Case
            A single test function that verifies one logical unit of behaviour.  
            A case may contain multiple assertions, but they all support testing the same 
            unit of functionality.

        **Special export values**:
            | Name   | Description                                       |
            | ------ | ------------------------------------------------- |
            | `init` | Always run before any test cases run in the suite |
\

fs:use`qlib.fs;

// @brief Get the current directory of the test module.
// @return {fileSymbol} Current directory.
// @example
//     q)unit.currentDir[]
//     `:/home/user/qlib/test/q
currentDir:{[] first ` vs first ` vs .Q.rp`:::};

// @brief Get the name of the current test suite.
// @return {symbol} Current test suite name.
// @example
//     q)unit.suiteName[]
//     `test_clap
suiteName:{[] fs.basenameNoExt first ` vs .Q.rp`:::};

// @brief Create a temporary directory.
// @return {fileSymbol} Path to temporary directory.
// @example
//     q)unit.createTmpDir[]
//     `:/tmp/qlib/unit/843739810360896131/run
createTmpDir:{[] fs.mkdir dir:.Q.dd[tmpDir;suiteName[]]; dir};

// @brief Register the test directory where all unit test files can be found.
// @param dir {fileSymbol} Directory to register.
// @example
//     unit.registerTestDir .Q.dd[unit.currentDir[];`unit]
registerTestDir:{[dir]
    files:key dir;
    (dir;parent):` vs dir;
    .Q.m.SP,:1_string dir;
    addSuite[parent;] each files;
    .Q.m.SP:-1_.Q.m.SP;
 };

// @brief Run all tests.
// @return {table} Test results.
// @example
//     q)unit.run[]
//     suite     case         start                         errInfo pass duration            
//     --------------------------------------------------------------------------------------
//     test_clap testRaw      2026.09.26D12:27:26.256728762 ::      1    0D00:00:00.000046610
//     test_clap testRegister 2026.09.26D12:27:26.256798277 ::      1    0D00:00:00.000103790
//     test_clap testParse    2026.09.26D12:27:26.256910504 ::      1    0D00:00:00.000384504
//     ..
run:{[] raze runSuite each 1_key suites};

// @brief Run all cases within the given suite.
// @param suite {symbol} Test suite to run.
// @return {table} Test results.
// @example
//     q)unit.runSuite `test_clap
//     suite     case            start                         errInfo pass duration            
//     -----------------------------------------------------------------------------------------
//     test_clap testRaw         2026.09.26D12:34:15.371053924 ::      1    0D00:00:00.000058313
//     test_clap testRegister    2026.09.26D12:34:15.371133478 ::      1    0D00:00:00.000065858
//     test_clap testParse       2026.09.26D12:34:15.371205828 ::      1    0D00:00:00.000259773
//     test_clap testParseErrors 2026.09.26D12:34:15.371472945 ::      1    0D00:00:00.000187383
runSuite:{[suite]
    caseNames:key suites suite;

    // Always run init first
    if[`init in caseNames;
        if[not (r:runCase[suite;`init])`pass; :enlist r];
        caseNames:caseNames except `init
    ];

    results:runCase[suite;] each caseNames;
    fs.rmrf .Q.dd[tmpDir;suite];
    results
 };

// @brief Run a single test case.
// @param suite {symbol} Suite that contains the test case.
// @param case {symbol} Test case to run.
// @return {table} Test results.
// @example
//     q)unit.runCase[`test_clap;`testRaw]
//     suite   | `test_clap
//     case    | `testRaw
//     start   | 2026.09.26D12:37:05.084200873
//     errInfo | ::
//     pass    | 1b
//     duration| 0D00:00:00.000043284
runCase:{[suite;case]
    caseFunc:suites[suite;case];
    res:([suite;case;start:.z.p]);
    res[`errInfo]:execute[suite;caseFunc];
    res,:([pass:(::)~res`errInfo; duration:.z.p-res`start]);
    resetMocks[];
    res
 };

// @brief Mock a value for the remainder of the test case (automatically restored).
// @param module {symbol} The full name of the module where the name to be mocked is defined.
// @param name {symbol} Name of value you want to mock.
// @param mockValue {any} The value that the name should hold.
// @example 
//     mock[`qlib.clap;`raw;{[] `blah}]
mock:{[module;name;mockValue]
    ns:.Q.m.mn module;
    name:` sv ns,name;
    if[(::)~mocks name; mocks[name]:get name];
    name set mockValue;
 };

// @brief Assert `x` is a truthy value.
// @param x {any} Q object.
// @example 
//     unit.assert.true 1b
assert.true:{
    if[not x; '"ASSERT TRUE | Expected ",.Q.s1[x]," to be a truthy value"]
 };

// @brief Assert `x` is a falsey value.
// @param x {any} Q object.
// @example 
//     unit.assert.false 0b
assert.false:{
    if[x; '"ASSERT FALSE | Expected ",.Q.s1[x]," to be a falsey value"]
 };

// @brief Assert that `x` equals `y`.
// @param x {any} Q object.
// @param y {any} Q object.
// @example 
//     unit.assert.equal[8;9]
assert.eq:{
    if[not x=y; '"ASSERT EQUAL | Expected ",.Q.s1[x]," = ",.Q.s1 y]
 };

// @brief Assert that `x` matches `y`.
// @param x {any} Q object.
// @param y {any} Q object.
// @example 
//     unit.assert.matches[8;9]
assert.match:{
    if[not x~y; '"ASSERT MATCH | Expected ",.Q.s1[x]," ~ ",.Q.s1 y]
 };

// @brief Assert that `x` like `y`.
// @param x {any} Q object.
// @param y {any} Q object.
// @example 
//     unit.assert.like["hello";"goodbye"]
assert.like:{
    if[not x like y; '"ASSERT LIKE | Expected ",.Q.s1[x]," like ",.Q.s1 y]
 };

// @brief Assert that function application raises an error.
// @param f {function} Function to apply.
// @param args {list} Function arguments.
// @param err {string} Expected error.
// @example 
//     unit.assert.fail[clap.parse;();"Missing argument(s): requiredArg"
// @example
//     unit.assert.fail[dbm.strToSymCol; (db;`trade;`size;`sym); "type"]
assert.fail:{[f;args;err]
    if[(0h>type args) or args~(); args:enlist args];
    r:.[
        {x . y; 1b};
        (f;args);
        {$[x like y; 0b; '"ASSERT FAIL | Expected error like ",.Q.s1[y],", but got ",.Q.s1 x]}[;err]
    ];
    if[r; '"ASSERT FAIL | Did not fail"];
 };

// @brief Enable or disable colour (ANSI escape codes) in printed output (disabled by default).
// @param enable {boolean} Whether to use colour.
// @example
//     unit.setColor 1b
setColor:{[enable] color::enable};

// @brief Print a summary of the results to standard output.
// @param results {table} Results table (returned from the `run` function).
// @example
//     unit.print.summary unit.run[]
print.summary:{[results]
    summary:`suite`case xasc select suite, case, duration, pass from results;
    passed:sum summary`pass;
    failed:sum not summary`pass;
    // Colour each row (after the header and separator lines) by its result
    table:.Q.s2 summary;
    table:(2#table),colorise'[`red`green summary`pass;2_table];
    stdout (""; "Unit Test Summary"),(indentSize#" "),/:table,(
        "";
        "Duration   | ",string sum summary`duration;
        "Test Count | ",string count summary;
        colorise[`none`green passed>0;"Passed     | ",string passed];
        colorise[`none`red failed>0;"Failed     | ",string failed];
        ""
    );
 };

// @brief Print failed test cases to standard output.
// @param results {table} Results table (returned from the `run` function).
// @example
//     unit.print.failures unit.run[]
print.failures:{[results]
    if[count failures:select from results where not pass;
        stdout (""; "Unit Test Failures"),(indentSize#" "),/:raze formatFailure each failures
    ]
 };

export:([
    createTmpDir; currentDir; suiteName; registerTestDir; run; runSuite; runCase; mock; assert;
    setColor; print
 ]);


tmpDir:.Q.dd[fs.tmpdir[];`qlib`unit,"j"$.z.p];

indentSize:4;
stdout:-1;

color:0b;
ansi:`green`red`reset!("\033[0;32m";"\033[0;31m";"\033[0m");

mocks:(1#`$())!1#(::);
suites:(1#`$())!1#(::);

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

// Wrap a string in the ANSI escape codes for colour c (`none, or colour disabled, leaves it as is)
colorise:{[c;str] $[color and c in key ansi; ansi[c],str,ansi`reset; str]};

resetMocks:{[]
    {key[x] set' value x} 1_mocks;
    mocks::1#mocks;
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
