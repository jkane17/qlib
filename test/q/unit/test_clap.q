
/
    @file test_clap.q
    @brief Unit tests for clap.q module.
\

clap:use`qlib.clap;
unit:use`qlib.unit;

atomArg:`name`default`required`validate`description!(`atomArg;10;0b;(::);"Atom argument");
listArg:`name`default`required`validate`description!(`listArg;1 2 3;0b;(::);"List argument");
stringArg:`name`default`required`validate`description!(
    `stringArg;enlist"Hello";0b;(::);"String argument"
 );
requiredArg:`name`default`required`validate`description!(
    `requiredArg;10;1b;(::);"Required argument"
 );
validatedArg:`name`default`required`validate`description!(
    `validatedArg;10;0b;11>;"Validated argument"
 );

registerArgs:{[]
    clap.register . value atomArg;
    clap.register . value listArg;
    clap.register . value stringArg;
    clap.register . value requiredArg;
    clap.register . value validatedArg;
 };

testRaw:{[] unit.assert.match[clap.raw[]; .Q.opt .z.X]; };

testRegister:{[]
    registerArgs[];

    unit.assert.match[
        `name`default`required`description#atomArg; 
        exec from clap.usage[] where name=`atomArg
    ];
    
    unit.assert.match[
        `name`default`required`description#listArg; 
        exec from clap.usage[] where name=`listArg
    ];

    unit.assert.match[
        `name`default`required`description#stringArg; 
        exec from clap.usage[] where name=`stringArg
    ];
        
    unit.assert.match[
        `name`default`required`description#requiredArg; 
        exec from clap.usage[] where name=`requiredArg
    ];

    unit.assert.match[
        `name`default`required`description#validatedArg; 
        exec from clap.usage[] where name=`validatedArg
    ];
 };

testParse:{[]
    registerArgs[];
    unit.mock[`qlib.clap;`raw;([requiredArg:enlist "15"])];

    clap.parse[];
   
    unit.assert.eq[clap.get`atomArg; 10];
    unit.assert.match[clap.get`listArg; 1 2 3];
    unit.assert.match[clap.get`stringArg; enlist"Hello"];
    unit.assert.eq[clap.get`requiredArg; 15];
    unit.assert.eq[clap.get`validatedArg; 10];

    unit.assert.false clap.given`atomArg;
    unit.assert.false clap.given`listArg;
    unit.assert.false clap.given`stringArg;
    unit.assert.true clap.given`requiredArg;
    unit.assert.false clap.given`validatedArg;

    unit.mock[`qlib.clap;`raw; ([
        atomArg:enlist "20"; 
        listArg:("10";"20";"30"); 
        stringArg:enlist "Goodbye"; 
        requiredArg:enlist "15";
        validatedArg:enlist "5"
    ])];

    clap.parse[];

    unit.assert.eq[clap.get`atomArg; 20];
    unit.assert.match[clap.get`listArg; 10 20 30];
    unit.assert.match[clap.get`stringArg; enlist "Goodbye"];
    unit.assert.eq[clap.get`requiredArg; 15];
    unit.assert.eq[clap.get`validatedArg; 5];

    unit.assert.true clap.given`atomArg;
    unit.assert.true clap.given`listArg;
    unit.assert.true clap.given`stringArg;
    unit.assert.true clap.given`requiredArg;
    unit.assert.true clap.given`validatedArg;
 };

testParseErrors:{[]
    registerArgs[];

    // Required arg not given
    unit.mock[`qlib.clap;`raw;([])];
    unit.assert.fail[clap.parse;();"Missing argument(s): requiredArg"];

    // Validated arg given does not pass validation
    unit.mock[`qlib.clap;`raw;([validatedArg: enlist "12"])];
    unit.assert.fail[clap.parse;();"Invalid argument(s): validatedArg"];
 };

export:([testRaw; testRegister; testParse; testParseErrors]);
