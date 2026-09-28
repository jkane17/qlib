
/
    @file test_fs.q

    @overview 
        Unit tests for the fs.q module.
\

fs:use`qlib.fs;
unit:use`qlib.unit;

testBasename:{[]
    {unit.assert.eq[fs.basename x; `]} each ``:;
    {unit.assert.eq[fs.basename x; `file]} each `file`:file;
    {unit.assert.eq[fs.basename x; `file.txt]} each `file.txt`:file.txt;
    {unit.assert.eq[fs.basename x; `]} each (`$"dir/"),`:dir/;
    {unit.assert.eq[fs.basename x; `file.txt]} each (`$"dir/file.txt"),`:dir/file.txt;
    {unit.assert.eq[fs.basename x; `]} each (`$"////"),`:////;
    {unit.assert.eq[fs.basename x; `file.txt]} each (`$"////file.txt"),`:////file.txt;
 };

export:([testBasename]);
