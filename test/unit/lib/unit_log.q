
/
    File        : unit_log.q
    Description : Test log.q
\

.pkg.load `fstr`log`unit;

.unit_log.dir:.Q.dd[.unit.internal.dataPath;`log];
.unit_log.outFile:.Q.dd[.unit_log.dir;`outFile];
.unit_log.errFile:.Q.dd[.unit_log.dir;`errFile];

.unit_log.msg:.fstr.fmt["{} message";];

.unit_log.logAll:{[]
    .log.trace .unit_log.msg`TRACE;
    .log.debug .unit_log.msg`DEBUG;
    .log.info  .unit_log.msg`INFO;
    .log.warn  .unit_log.msg`WARN;
    .log.error .unit_log.msg`ERROR;
    .log.fatal .unit_log.msg`FATAL;
 };

.unit_log.checkLine:{[lvls;line]
    line:" " vs line;
    lvl:`$line[1] where line[1] in .Q.A;
    
    .unit.assert.like[line 0;"????.??.??D??:??:??.?????????"];
    .unit.assert.in[lvl;lvls];
    .unit.assert.match[line 2;enlist ":"];
    .unit.assert.match[" " sv line 3 4;.unit_log.msg lvl];
 };

.unit_log.checkLevel:{[lvl]
    .log.setStdout out:hopen .unit_log.outFile;
    .log.setStderr err:hopen .unit_log.errFile;
    
    .log.setLvl lvl;
    .unit_log.logAll[];
    
    outData:read0 .unit_log.outFile;
    errData:read0 .unit_log.errFile;

    .unit.assert.match[lvl;.log.getLvl[]];
    .unit.assert.match[count outData;] (.log.internal.lvls!4 3 2 1 0 0 0) lvl;
    .unit.assert.match[count errData;] (.log.internal.lvls!2 2 2 2 2 1 0) lvl;
    
    lvls:last (enlist .log.internal.lvls?lvl) cut .log.internal.lvls;
    .unit_log.checkLine[lvls;] each outData;
    .unit_log.checkLine[lvls;] each errData;

    hclose each out,err;
    .log.setLvl `OFF;
    hdel each .unit_log`outFile`errFile;
    .log.setLvl lvl;
 };

levels:{[]
    outOrig:.log.getStdout[];
    errOrig:.log.getStderr[];
    orig:.log.getLvl[];

    .unit_log.checkLevel each .log.internal.lvls;    
    
    .log.setLvl orig;
    .log.setStdout outOrig;
    .log.setStderr errOrig;
    hdel .unit_log.dir;
 };

.unit.add[`log;] each `levels;
