
/
    @file
        unit_os.q
    
    @description
        Unit tests for os.q
\

.qlib.module.load `os`unit;

cp:{[]
    // Copy a file
    src:.Q.dd[.unit.internal.dataPath;`sample.csv];
    dst:.Q.dd[.unit.internal.dataPath;`sampleCp.csv];
    .unit.assert.match[src;key src];
    .unit.assert.match[();key dst];
    .os.cp[src;dst];
    .unit.assert.match[dst;key dst];
    hdel dst; // Clean-up

    // Copy a directory
    src:.Q.dd[.unit.internal.dataPath;`dir1];
    dst:.Q.dd[.unit.internal.dataPath;`dir1Cp];
    .unit.assert.notEmpty key src;
    .unit.assert.match[();key dst];
    .os.cp[src;dst];
    .unit.assert.notEmpty key dst;
    .os.rmrf dst; // Clean-up

    // Copy a non-existent file - Should fail
    .os.hideStderr 1b;
    src:.Q.dd[.unit.internal.dataPath;`nonExistFile.csv];
    dst:.Q.dd[.unit.internal.dataPath;`nonExistFileCp.csv];
    .unit.assert.match[();key src];
    .unit.assert.match[();key dst];
    .unit.assert.fail[.os.cp;(src;dst);"os"];
    .unit.assert.match[();key src];
    .unit.assert.match[();key dst];
    .os.hideStderr 0b;
 };

exists:{[]
    .unit.assert.true .os.exists .Q.dd[.unit.internal.dataPath;`sample.csv];
    .unit.assert.true .os.exists .Q.dd[.unit.internal.dataPath;`.hidden];
    .unit.assert.true .os.exists .Q.dd[.unit.internal.dataPath;`dir1];
    .unit.assert.false .os.exists .Q.dd[.unit.internal.dataPath;`nonExistingFile.txt];
 };

mkdir:{[]
    // Create a new directory
    newDir:.Q.dd[.unit.internal.dataPath;`newDir];
    .unit.assert.match[();key newDir];
    .unit.assert.match[newDir;.os.mkdir newDir];
    .unit.assert.match[`$();key newDir];
    .os.rmrf newDir;

    // Make a subdirectory even though parent directory does not yet exist
    rootDir:.Q.dd[.unit.internal.dataPath;`rootDir];
    subDir:.Q.dd[rootDir;`subDir];
    .unit.assert.match[();key rootDir];
    .unit.assert.match[subDir;.os.mkdir subDir];
    .unit.assert.match[1#`subDir;key rootDir];
    .unit.assert.match[`$();key subDir];
    .os.rmrf rootDir;

    // Make a directory that already exists
    dir2:.Q.dd[.unit.internal.dataPath;`dir2];
    .unit.assert.match[`$();key dir2];
    .unit.assert.match[dir2;.os.mkdir dir2];
    .unit.assert.match[`$();key dir2];
 };

mv:{[]
    // Move a file
    src:.Q.dd[.unit.internal.dataPath;`sample.csv];
    dst:.Q.dd[.unit.internal.dataPath;`dir2`sample.csv];
    .unit.assert.match[src;key src];
    .unit.assert.match[();key dst];
    .os.mv[src;dst];
    .unit.assert.match[();key src];
    .unit.assert.match[dst;key dst];
    .os.mv[dst;src]; // Clean-up - Move it back
    .unit.assert.match[src;key src];
    .unit.assert.match[();key dst];

    // Rename a file
    dst:.Q.dd[.unit.internal.dataPath;`sampleMv.csv];
    .unit.assert.match[();key dst];
    .os.mv[src;dst];
    .unit.assert.match[();key src];
    .unit.assert.match[dst;key dst];
    .os.mv[dst;src]; // Clean-up - Rename to original
    .unit.assert.match[src;key src];
    .unit.assert.match[();key dst];

    // Move a directory
    src:.Q.dd[.unit.internal.dataPath;`dir1];
    dst:.Q.dd[.unit.internal.dataPath;`dir2`dir1];
    .unit.assert.notEmpty key src;
    .unit.assert.match[();key dst];
    .os.mv[src;dst];
    .unit.assert.match[();key src];
    .unit.assert.notEmpty key dst;
    .os.mv[dst;src]; // Clean-up - Move it back
    .unit.assert.notEmpty key src;
    .unit.assert.match[();key dst];

    // Rename a directory
    dst:.Q.dd[.unit.internal.dataPath;`dir1Mv];
    .unit.assert.match[();key dst];
    .os.mv[src;dst];
    .unit.assert.match[();key src];
    .unit.assert.notEmpty key dst;
    .os.mv[dst;src]; // Clean-up - Rename to original
    .unit.assert.notEmpty key src;
    .unit.assert.match[();key dst];

    // Move a non-existent file - Should fail
    .os.hideStderr 1b;
    src:.Q.dd[.unit.internal.dataPath;`nonExistFile.csv];
    dst:.Q.dd[.unit.internal.dataPath;`nonExistFileMv.csv];
    .unit.assert.match[();key src];
    .unit.assert.match[();key dst];
    .unit.assert.fail[.os.mv;(src;dst);"os"];
    .unit.assert.match[();key src];
    .unit.assert.match[();key dst];
    .os.hideStderr 0b;
 };

rls:{[]
    // Directory with subdirectories
    path:.Q.dd[.unit.internal.dataPath;`dir1];
    expected:.Q.dd[path;] each (`dir11;`dir11`file11.txt;`dir12;`file1.csv);
    .unit.assert.match[expected;.os.rls path];

    // Empty directory
    path:.Q.dd[.unit.internal.dataPath;`dir2];
    .unit.assert.match[`$();key path];
    .unit.assert.match[`$();.os.rls path];

    // Non-existent directory
    .unit.assert.match[();.os.rls .Q.dd[.unit.internal.dataPath;`nonExistDir]];
 };

rm:{[]
    // Remove a file
    file:.Q.dd[.unit.internal.dataPath;`rmFile.csv];
    file 0: csv 0: ([] a:1 2 3; b:4 5 6);
    .unit.assert.match[file;key file];
    .os.rm file;
    .unit.assert.match[();key file];

    // Remove a non-existent file
    file:.Q.dd[.unit.internal.dataPath;`rmNonExistFile.csv];
    .unit.assert.match[();key file];
    .os.rm file;
    .unit.assert.match[();key file];
 };

rmrf:{[]
    data:csv 0: ([] a:1 2 3; b:4 5 6);

    // Remove a file
    file:.Q.dd[.unit.internal.dataPath;`rmrfFile.csv];
    file 0: data;
    .unit.assert.match[file;key file];
    .os.rmrf file;
    .unit.assert.match[();key file];

    // Directory and all its contents
    dir:.Q.dd[.unit.internal.dataPath;`rmrfDir];
    file1:.Q.dd[dir;`rmrfFile.csv];
    file1 0: data;
    file2:.Q.dd[dir;`subDir`rmrfFile.csv];
    file2 0: data;
    .unit.assert.match[`rmrfFile.csv`subDir;key dir];
    .unit.assert.match[1#`rmrfFile.csv;key .Q.dd[dir;`subDir]];
    .os.rmrf dir;
    .unit.assert.match[();key dir];

    // Remove a non-existent file
    file:.Q.dd[.unit.internal.dataPath;`rmrfNonExistFile.csv];
    .unit.assert.match[();key file];
    .os.rmrf file;
    .unit.assert.match[();key file];
 };

.unit.add[`os;] each `cp`exists`mkdir`mv`rls`rm`rmrf;
