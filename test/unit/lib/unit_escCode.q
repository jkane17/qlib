
/
    File:
        unit_escCode.q
    
    Description:
        Unit tests for escCode.q
\

.pkg.load `escCode`unit;

apply:{[]
    codes:.pkg.internal.getCnfMap `escCode;
    esc:.escCode.priv.esc;
    text:"Hello World";

    expected:esc,"[",string[codes`BOLD],"m",text,esc,"[",string[codes`RESET],"m";
    .unit.assert.match[expected;.escCode.apply[`BOLD;text]];

    expected:esc,"[",string[codes`BOLD],";",string[codes`FG_RED],
        "m",text,esc,"[",string[codes`RESET],"m";
    .unit.assert.match[expected;.escCode.apply[`BOLD`FG_RED;text]];

    expected:esc,"[",string[codes`FG_RED],";",string[codes`BOLD],
        "m",text,esc,"[",string[codes`RESET],"m";
    .unit.assert.match[expected;.escCode.apply[`FG_RED`BOLD;text]];
 };

listAll:{[]
    .unit.assert.type[11h;.escCode.listAll[]];
 };

listFGColors:{[]
    colors:.escCode.listFGColors[];
    .unit.assert.type[11h;colors];
    .unit.assert.true all colors like "FG_*";
 };

listBGColors:{[]
    colors:.escCode.listBGColors[];
    .unit.assert.type[11h;colors];
    .unit.assert.true all colors like "BG_*";
 };

listColors:{[]
    colors:.escCode.listColors[];
    .unit.assert.type[11h;colors];
    .unit.assert.true all any colors like/: ("FG_*";"BG_*");
 };

.unit.add[`escCode;] each `apply`listAll`listFGColors`listBGColors`listColors;
