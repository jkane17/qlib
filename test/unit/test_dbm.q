
unit:use`qlib.unit;
fs:use`qlib.fs;
unit.loadSrc `dbm.q;

tmp:unit.createTmpDir[];
splayDB:.Q.dd[tmp;`splayDB];
partDB:.Q.dd[tmp;`partDB];
splayTdir:.Q.dd[splayDB;`trade];
partDates:.z.d-1 0;
partTdirs:{.Q.dd[partDB;x,`trade]} each partDates;

init:{[]
    fs.rmrf splayDB;
    fs.rmrf partDB;
    fs.mkdir splayDB;
    fs.mkdir partDB;

    .z.m.trade:([]
        time:5#.z.P;
        sym:`IBM`AMZN`GOOGL`META`SPOT;
        size:1 2 3 4 5;
        price:10 20 30 40 50f;
        company:(
            "International Business Machines Corporation";
            "Amazon.com, Inc.";
            "Alphabet Inc.";
            "Meta Platforms, Inc.";
            "Spotify Technology S.A."
        );
        moves:3 cut -5+15?10
    );

    .Q.dd[splayDB;`trade`] set .Q.en[splayDB;.z.m.trade];

    {[db;dt;tname] 
        .Q.dd[db;dt,tname,`] set .Q.en[db;.z.m tname]
    }[partDB;;`trade] each partDates;

    delete trade from `.;
 };

testListCols:{[]
    init[];
    
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[splayDB;`trade]; colNames];
    unit.assert.match[listCols[partDB;`trade]; colNames];
    unit.assert.match[listCols[`:nonExistingDB;`trade]; `$()];
    unit.assert.match[listCols[splayDB;`nonExistingTable]; `$()];
 };

testAddColSplay:{[]
    init[];

    // No affect since column already exists
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[splayDB;`trade]; colNames];
    addCol[splayDB;`sym;`trade;`size;0N];
    unit.assert.match[listCols[splayDB;`trade]; colNames];

    addCol[splayDB;`sym;`trade;`newCol;0N];
    unit.assert.true `newCol in key splayTdir;
    unit.assert.true `newCol in get splayTdir,`.d;
    unit.assert.true all 0N=get splayTdir,`newCol;
    unit.assert.match[listCols[splayDB;`trade]; colNames,`newCol];
 };

testAddColPart:{[]
    init[];

    // No affect since column already exists
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[partDB;`trade]; colNames];
    addCol[partDB;`sym;`trade;`size;0N];
    unit.assert.match[listCols[partDB;`trade]; colNames];

    addCol[partDB;`sym;`trade;`newCol;0N];
    {
        unit.assert.true `newCol in key x;
        unit.assert.true `newCol in get x,`.d;
        unit.assert.true all 0N=get x,`newCol
    } each partTdirs;
    unit.assert.match[listCols[partDB;`trade]; colNames,`newCol];
 };

testDelColSplay:{[]
    init[];

    // No affect since column does not exist
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[splayDB;`trade]; colNames];
    delCol[splayDB;`trade;`nonExistingCol];
    unit.assert.match[listCols[splayDB;`trade]; colNames];

    delCol[splayDB;`trade;`size];
    unit.assert.false `size in key splayTdir;
    unit.assert.false `size in get splayTdir,`.d;
    unit.assert.match[listCols[splayDB;`trade]; colNames except `size];

    // Delete nested - should delete associated # file
    unit.assert.true all (`company,`$"company#") in key splayTdir;
    delCol[splayDB;`trade;`company];
    unit.assert.false all (`company,`$"company#") in key splayTdir;
    unit.assert.match[listCols[splayDB;`trade]; colNames except `size`company];
 };

testDelColPart:{[]
    init[];

    // No affect since column does not exist
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[partDB;`trade]; colNames];
    delCol[partDB;`trade;`nonExistingCol];
    unit.assert.match[listCols[partDB;`trade]; colNames];

    delCol[partDB;`trade;`size];
    {
        unit.assert.false `size in key x;
        unit.assert.false `size in get x,`.d;
    } each partTdirs;
    unit.assert.match[listCols[partDB;`trade]; colNames except `size];

    // Delete nested - should delete associated # file
    {unit.assert.true all (`company,`$"company#") in key x} each partTdirs;
    delCol[partDB;`trade;`company];
    {unit.assert.false all (`company,`$"company#") in key x} each partTdirs;
    unit.assert.match[listCols[partDB;`trade]; colNames except `size`company];
 };

testCopyColSplay:{[]
    init[];

    // No affect since price already exists
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[splayDB;`trade]; colNames];
    copyCol[splayDB;`trade;`size;`price];
    unit.assert.match[listCols[splayDB;`trade]; colNames];

    copyCol[splayDB;`trade;`size;`sizeCopy];
    unit.assert.true `sizeCopy in key splayTdir;
    unit.assert.true `sizeCopy in get splayTdir,`.d;
    unit.assert.match[get splayTdir,`size;get splayTdir,`sizeCopy];
    unit.assert.match[listCols[splayDB;`trade]; colNames,`sizeCopy];

    // Copy nested - should copy associated # file
    copyCol[splayDB;`trade;`company;`companyCopy];
    unit.assert.true all (`companyCopy,`$"companyCopy#") in key splayTdir;
    unit.assert.match[get splayTdir,`companyCopy;get splayTdir,`companyCopy];
    unit.assert.match[get splayTdir,`$"companyCopy#";get splayTdir,`$"companyCopy#"];
    unit.assert.match[listCols[splayDB;`trade]; colNames,`sizeCopy`companyCopy];
 };

testCopyColPart:{[]
    init[];

    // No affect since price already exists
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[partDB;`trade]; colNames];
    copyCol[partDB;`trade;`size;`price];
    unit.assert.match[listCols[partDB;`trade]; colNames];

    copyCol[partDB;`trade;`size;`sizeCopy];
    {
        unit.assert.true `sizeCopy in key x;
        unit.assert.true `sizeCopy in get x,`.d;
        unit.assert.match[get x,`size;get x,`sizeCopy];
    } each partTdirs;
    unit.assert.match[listCols[partDB;`trade]; colNames,`sizeCopy];

    // Copy nested - should copy associated # file
    copyCol[partDB;`trade;`company;`companyCopy];
    {
        unit.assert.true all (`companyCopy,`$"companyCopy#") in key x;
        unit.assert.match[get x,`companyCopy;get x,`companyCopy];
        unit.assert.match[get x,`$"companyCopy#";get x,`$"companyCopy#"];
    } each partTdirs;
    unit.assert.match[listCols[partDB;`trade]; colNames,`sizeCopy`companyCopy];
 };

testHasCol:{[]
    init[];
    
    unit.assert.true hasCol[splayDB;`trade;`size];
    unit.assert.false hasCol[splayDB;`trade;`nonExistingCol];
    unit.assert.true hasCol[partDB;`trade;`size];
    unit.assert.false hasCol[partDB;`trade;`nonExistingCol];
 };

testRenameColSplay:{[]
    init[];

    // No affect since price already exists
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[splayDB;`trade]; colNames];
    renameCol[splayDB;`trade;`size;`price];
    unit.assert.match[listCols[splayDB;`trade]; colNames];

    sizeData:get splayTdir,`size;
    renameCol[splayDB;`trade;`size;`sizeRenamed];
    unit.assert.true `sizeRenamed in key splayTdir;
    unit.assert.true `sizeRenamed in get splayTdir,`.d;
    unit.assert.false `size in key splayTdir;
    unit.assert.false `size in get splayTdir,`.d;
    unit.assert.match[sizeData;get splayTdir,`sizeRenamed];
    colNames:@[colNames;where colNames=`size;:;`sizeRenamed];
    unit.assert.match[listCols[splayDB;`trade]; colNames];

    // Rename nested - should rename associated # file
    companyData:get splayTdir,`company;
    companyHashData:get splayTdir,`$"company#";
    renameCol[splayDB;`trade;`company;`companyRenamed];
    unit.assert.true all (`companyRenamed,`$"companyRenamed#") in key splayTdir;
    unit.assert.match[companyData;get splayTdir,`companyRenamed];
    unit.assert.match[companyHashData;get splayTdir,`$"companyRenamed#"];
    colNames:@[colNames;where colNames=`company;:;`companyRenamed];
    unit.assert.match[listCols[splayDB;`trade]; colNames];
 };

testRenameColPart:{[]
    init[];

    // No affect since price already exists
    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[partDB;`trade]; colNames];
    renameCol[partDB;`trade;`size;`price];
    unit.assert.match[listCols[partDB;`trade]; colNames];

    sizeData:get partTdirs[0],`size;
    renameCol[partDB;`trade;`size;`sizeRenamed];
    {
        unit.assert.true `sizeRenamed in key x;
        unit.assert.true `sizeRenamed in get x,`.d;
        unit.assert.false `size in key x;
        unit.assert.false `size in get x,`.d;
        unit.assert.match[y;get x,`sizeRenamed];
    }[;sizeData] each partTdirs;
    colNames:@[colNames;where colNames=`size;:;`sizeRenamed];
    unit.assert.match[listCols[partDB;`trade]; colNames];

    // Rename nested - should rename associated # file
    companyData:get partTdirs[0],`company;
    companyHashData:get partTdirs[0],`$"company#";
    renameCol[partDB;`trade;`company;`companyRenamed];
    {
        unit.assert.true all (`companyRenamed,`$"companyRenamed#") in key x;
        unit.assert.match[y;get x,`companyRenamed];
        unit.assert.match[z;get x,`$"companyRenamed#"];
    }[;companyData;companyHashData] each partTdirs;
    colNames:@[colNames;where colNames=`company;:;`companyRenamed];
    unit.assert.match[listCols[partDB;`trade]; colNames];
 };

testReorderColsSplay:{[]
    init[];

    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[splayDB;`trade]; colNames];

    unit.assert.fail[
        reorderCols;
        (splayDB;`trade;colNames,`unknownCol);
        "Unknown column(s): unknownCol"
    ];

    reorderCols[splayDB;`trade;reverse colNames];
    unit.assert.match[listCols[splayDB;`trade]; reverse colNames];

    // Only named columns are reordered
    reorderCols[splayDB;`trade;`sym`company`time];
    unit.assert.match[listCols[splayDB;`trade]; `sym`company`time`moves`price`size];
 };

testReorderColsPart:{[]
    init[];

    colNames:`time`sym`size`price`company`moves;
    unit.assert.match[listCols[partDB;`trade]; colNames];

    unit.assert.fail[
        reorderCols;
        (partDB;`trade;colNames,`unknownCol);
        "Unknown column(s): unknownCol"
    ];

    reorderCols[partDB;`trade;reverse colNames];
    unit.assert.match[listCols[partDB;`trade]; reverse colNames];

    // Only named columns are reordered
    reorderCols[partDB;`trade;`sym`company`time];
    unit.assert.match[listCols[partDB;`trade]; `sym`company`time`moves`price`size];
 };

testFnColSplay:{[]
    init[];

    // No affect since column does not exist
    fnCol[splayDB;`trade;`nonExistingCol;10*];

    sizeData:get splayTdir,`size;
    fnCol[splayDB;`trade;`size;10*];
    unit.assert.match[10*sizeData;get splayTdir,`size];

    companyData:get splayTdir,`company;
    fnCol[splayDB;`trade;`company;upper];
    unit.assert.match[upper companyData;get splayTdir,`company];
 };

testFnColPart:{[]
    init[];

    // No affect since column does not exist
    fnCol[partDB;`trade;`nonExistingCol;10*];

    sizeData:get partTdirs[0],`size;
    companyData:get partTdirs[0],`company;

    fnCol[partDB;`trade;`size;10*];
    fnCol[partDB;`trade;`company;upper];
    {
        unit.assert.match[10*y;get x,`size];
        unit.assert.match[upper z;get x,`company];
    }[;sizeData;companyData] each partTdirs;
 };

testCastColSplay:{[]
    init[];

    // No affect since column does not exist
    castCol[splayDB;`trade;`nonExistingCol;10*];

    unit.assert.eq[7h;type get splayTdir,`size];
    castCol[splayDB;`trade;`size;"f"];
    unit.assert.eq[9h;type get splayTdir,`size];
 };

testCastColPart:{[]
    init[];

    // No affect since column does not exist
    castCol[partDB;`trade;`nonExistingCol;10*];

    {unit.assert.eq[7h;type get x,`size]} each partTdirs;
    castCol[partDB;`trade;`size;"f"];
    {unit.assert.eq[9h;type get x,`size]} each partTdirs;
 };

testAttrSplay:{[]
    init[];

    // No affect since column does not exist
    setAttr[splayDB;`trade;`nonExistingCol;`s];

    unit.assert.eq[`;attr get splayTdir,`size];
    setAttr[splayDB;`trade;`size;`s];
    unit.assert.eq[`s;attr get splayTdir,`size];

    rmAttr[splayDB;`trade;`size];
    unit.assert.eq[`;attr get splayTdir,`size];
 };

testAttrPart:{[]
    init[];

    // No affect since column does not exist
    setAttr[partDB;`trade;`nonExistingCol;`s];

    {unit.assert.eq[`;attr get x,`size]} each partTdirs;
    setAttr[partDB;`trade;`size;`s];
    {unit.assert.eq[`s;attr get x,`size]} each partTdirs;

    rmAttr[partDB;`trade;`size];
    {unit.assert.eq[`;attr get x,`size]} each partTdirs;
 };

testAddMissingColsSplay:{[]
    init[];

    colNames:`time`sym`size`price`company`moves;
    goodTdir:`$string[splayTdir],"Copy";
    fs.rcopy[splayTdir;goodTdir];
    
    // Single missing column
    unit.assert.match[listCols[splayDB;`trade]; colNames];
    delCol[splayDB;`trade;`size];
    unit.assert.match[listCols[splayDB;`trade]; colNames except `size];
    addMissingCols[splayDB;`trade;goodTdir];
    unit.assert.match[listCols[splayDB;`trade]; colNames];

    // Multiple missing columns (including nested)
    delCol[splayDB;`trade;] each `size`price`company;
    unit.assert.match[listCols[splayDB;`trade]; colNames except `size`price`company];
    addMissingCols[splayDB;`trade;goodTdir];
    unit.assert.match[listCols[splayDB;`trade]; colNames];
    unit.assert.true all (`company,`$"company#") in key splayTdir;
 };

testAddMissingColsPart:{[]
    init[];

    colNames:`time`sym`size`price`company`moves;
    goodTdir:`$string[partTdirs 0],"Copy";
    fs.rcopy[partTdirs 0;goodTdir];
    
    // Single missing column
    unit.assert.match[listCols[partDB;`trade]; colNames];
    delCol[partDB;`trade;`size];
    unit.assert.match[listCols[partDB;`trade]; colNames except `size];
    addMissingCols[partDB;`trade;goodTdir];
    unit.assert.match[listCols[partDB;`trade]; colNames];

    // Multiple missing columns (including nested)
    delCol[partDB;`trade;] each `size`price`company;
    unit.assert.match[listCols[partDB;`trade]; colNames except `size`price`company];
    addMissingCols[partDB;`trade;goodTdir];
    unit.assert.match[listCols[partDB;`trade]; colNames];
    {unit.assert.true all (`company,`$"company#") in key x} each partTdirs;
 };

testAddTabSplay:{[]
    init[];

    schema:([] sym:`$(); ap:"f"$(); bp:"f"$());
    unit.assert.false `quote in key splayDB;
    
    addTab[splayDB;`sym;`quote;schema];
    unit.assert.true `quote in key splayDB;
    unit.assert.match[listCols[splayDB;`quote]; `sym`ap`bp];
 };

testAddTabPart:{[]
    init[];

    schema:([] sym:`$(); ap:"f"$(); bp:"f"$());
    {unit.assert.false `quote in key .Q.dd[x;y]}[partDB;] each partDates;
    
    addTab[partDB;`sym;`quote;schema];
    {unit.assert.true `quote in key .Q.dd[x;y]}[partDB;] each partDates;
    unit.assert.match[listCols[partDB;`quote]; `sym`ap`bp];
 };

testDelTabSplay:{[]
    init[];

    unit.assert.true `trade in key splayDB;
    delTab[splayDB;`trade];
    unit.assert.false `trade in key splayDB;
 };

testDelTabPart:{[]
    init[];

    {unit.assert.true `trade in key .Q.dd[x;y]}[partDB;] each partDates;
    delTab[partDB;`trade];
    {unit.assert.false `trade in key .Q.dd[x;y]}[partDB;] each partDates;
 };

testRenameTabSplay:{[]
    init[];

    colNames:`time`sym`size`price`company`moves;

    unit.assert.true `trade in key splayDB;
    renameTab[splayDB;`trade;`tradeRenamed];
    unit.assert.false `trade in key splayDB;
    unit.assert.true `tradeRenamed in key splayDB;
    unit.assert.match[listCols[splayDB;`tradeRenamed]; colNames];
 };

testRenameTabPart:{[]
    init[];

    colNames:`time`sym`size`price`company`moves;

    {unit.assert.true `trade in key .Q.dd[x;y]}[partDB;] each partDates;
    renameTab[partDB;`trade;`tradeRenamed];
    {unit.assert.false `trade in key .Q.dd[x;y]}[partDB;] each partDates;
    {unit.assert.true `tradeRenamed in key .Q.dd[x;y]}[partDB;] each partDates;
    unit.assert.match[listCols[partDB;`tradeRenamed]; colNames];
 };

export:([
    testListCols; testAddColSplay; testAddColPart; testDelColSplay; testDelColPart;
    testCopyColSplay; testCopyColPart; testHasCol; testRenameColSplay; testRenameColPart;
    testReorderColsSplay; testReorderColsPart; testFnColSplay; testFnColPart; 
    testCastColSplay; testCastColPart; testAttrSplay; testAttrPart; 
    testAddMissingColsSplay; testAddMissingColsPart; testAddTabSplay; testAddTabPart;
    testDelTabSplay; testDelTabPart; testRenameTabSplay; testRenameTabPart
 ]);
