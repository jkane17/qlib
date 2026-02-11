
unit:use`qlib.unit;
unit.loadSrc `clap.q;

registerArgs:{[ns;args]
    unit.mock[ns; `args; 1#args]; // Clear args
    register[`atomArg;10;0b;(::);"Atom argument"];
    register[`listArg;1 2 3;0b;(::);"List argument"];
    register[`stringArg;enlist"Hello";0b;(::);"String argument"];
    register[`requiredArg;10;1b;(::);"Required argument"];
    register[`validatedArg;10;0b;11>;"Validated argument"];
 };

testRegister:{[]
    registerArgs[.z.M;.z.m.args];
    res:([val:(); default:10; required:0b; given:0b; validate:(::); description:"Atom argument"]);

    unit.assert.match[args`atomArg; res];

    res,:([default:1 2 3; description:"List argument"]);
    unit.assert.match[args`listArg; res];

    res,:([default:enlist"Hello"; description:"String argument"]);
    unit.assert.match[args`stringArg; res];

    res,:([default:10; required:1b; description:"Required argument"]);
    unit.assert.match[args`requiredArg; res];

    res,:([required:0b; validate:11>; description:"Validated argument"]);
    unit.assert.match[args`validatedArg; res];
 };

testRaw:{[]
    unit.assert.match[raw[]; .Q.opt .z.X];
 };

testParse:{[]
    registerArgs[.z.M;.z.m.args];

    // No args given (except required) - should use defaults
    unit.mock[.z.M; `raw; ([requiredArg:enlist "15"])];
    .z.m.parse[];
    unit.assert.eq[.z.m.get`atomArg; 10];
    unit.assert.match[.z.m.get`listArg; 1 2 3];
    unit.assert.match[.z.m.get`stringArg; enlist"Hello"];
    unit.assert.eq[.z.m.get`requiredArg; 15];
    unit.assert.eq[.z.m.get`validatedArg; 10];

    unit.assert.false given`atomArg;
    unit.assert.false given`listArg;
    unit.assert.false given`stringArg;
    unit.assert.true given`requiredArg;
    unit.assert.false given`validatedArg;

    // Args given - should have correct types
    registerArgs[.z.M;.z.m.args];
    unit.mock[.z.M; `raw; ([
        atomArg:enlist "20"; 
        listArg:("10";"20";"30"); 
        stringArg:enlist "Goodbye"; 
        requiredArg:enlist "15";
        validatedArg:enlist "5"
    ])];
    .z.m.parse[];
    unit.assert.eq[.z.m.get`atomArg; 20];
    unit.assert.match[.z.m.get`listArg; 10 20 30];
    unit.assert.match[.z.m.get`stringArg; enlist "Goodbye"];
    unit.assert.eq[.z.m.get`requiredArg; 15];
    unit.assert.eq[.z.m.get`validatedArg; 5];

    unit.assert.true given`atomArg;
    unit.assert.true given`listArg;
    unit.assert.true given`stringArg;
    unit.assert.true given`requiredArg;
    unit.assert.true given`validatedArg;
 };

testParseErrors:{[]
    registerArgs[.z.M;.z.m.args];

    // Required arg not given
    unit.mock[.z.M; `raw; ([])];
    unit.assert.fail[.z.m.parse;();"Missing argument(s): requiredArg"];

    // Validated arg given does not pass validation
    unit.mock[.z.M; `raw; ([validatedArg: enlist "12"])];
    unit.assert.fail[.z.m.parse;();"Invalid argument(s): validatedArg"];
 };

testUsage:{[]
    registerArgs[.z.M;.z.m.args];
    unit.assert.match[
        usage[];
        .Q.s2 ([]
            name:`atomArg`listArg`stringArg`requiredArg`validatedArg;
            default:(10;1 2 3;enlist"Hello";10;10);
            required:00010b;
            description:(
                "Atom argument";
                "List argument";
                "String argument";
                "Required argument";
                "Validated argument"
            )
        )
    ];
 };

export:([testRegister; testRaw; testParse; testParseErrors; testUsage]);
