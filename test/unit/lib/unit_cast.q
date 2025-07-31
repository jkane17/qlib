
/
    @file
        unit_cast.q
    
    @description
        Unit tests for cast.q
\

.qlib.module.load `cast`unit;

test_htostr:{[]
    expected:"hello/world";
    .unit.assert.match[expected;.cast.htostr `:hello/world];
    .unit.assert.match[expected;.cast.htostr  `$"hello/world"];
    .unit.assert.match[expected;.cast.htostr "hello/world"];
    .unit.assert.match["10";.cast.htostr `:10];
 };

test_tostr:{[]
    data:"C" _ .unit.randData 3;
    expected:string each data;
    .unit.assert.match[expected;.cast.tostr data];
    .unit.assert.match["my string";.cast.tostr "my string"];
 };

test_tosym:{[]
    data:.unit.randData 3;
    expected:`$string each data;
    .unit.assert.match[expected;.cast.tosym data];
 };

test_tohsym:{[]
    expected:`:hello/world;
    .unit.assert.match[expected;.cast.tohsym `:hello/world];
    .unit.assert.match[expected;.cast.tohsym `$"hello/world"];
    .unit.assert.match[expected;.cast.tohsym "hello/world"];
    .unit.assert.match[`:10;.cast.tohsym 10];
 };

.unit.add[`cast;] each `test_htostr`test_tostr`test_tosym`test_tohsym;
