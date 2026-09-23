
/
    @file test_dbm.q
    @brief Unit tests for dbm.q module.
\

dbm:use`qlib.dbm;
fs:use`qlib.fs;
unit:use`qlib.unit;
/ unit.loadSrc `dbm.q;

tmp:unit.createTmpDir[];
splayDB:.Q.dd[tmp;`splayDB];
partDB:.Q.dd[tmp;`partDB];
splayTdir:.Q.dd[splayDB;`trade];
partDates:.z.d-1 0;
partTdirs:{.Q.dd[partDB;x,`trade]} each partDates;
colNames:`time`sym`venue`size`price`company`moves;

init:{[]
    fs.rmrf splayDB;
    fs.rmrf partDB;
    fs.mkdir splayDB;
    fs.mkdir partDB;

    trade::([]
        time:5#.z.P;
        sym:`IBM`AMZN`GOOGL`META`SPOT;
        venue:`NYSE`NASDAQ`IEX`NYSE`IEX;
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

    .Q.dd[splayDB;`trade`] set .Q.en[splayDB;trade];
    {.Q.dd[partDB;x,`trade,`] set .Q.en[partDB;trade]} each partDates;

    delete sym, trade from `.;
 };

testListCols:{[]
    init[];
    
    unit.assert.match[dbm.listCols[splayDB;`trade]; colNames];
    unit.assert.match[dbm.listCols[partDB;`trade]; colNames];
    unit.assert.match[dbm.listCols[`:nonExistingDB;`trade]; `$()];
    unit.assert.match[dbm.listCols[splayDB;`nonExistingTable]; `$()];
 };

testAddCol:{[]
    init[];

    check:{[db;tdirs]
        // No affect since column already exists
        unit.assert.match[dbm.listCols[db;`trade]; colNames];
        dbm.addCol[db;`sym;`trade;`size;0N];
        unit.assert.match[dbm.listCols[db;`trade]; colNames];

        dbm.addCol[db;`sym;`trade;`newCol;0N];
        {
            unit.assert.true `newCol in key x;
            unit.assert.true `newCol in get x,`.d;
            unit.assert.true all 0N=get x,`newCol
        } each tdirs;

        unit.assert.match[dbm.listCols[db;`trade]; colNames,`newCol];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testAddCols:{[]
    init[];

    check:{[db;tdirs]
        // No affect since column already exists
        unit.assert.match[dbm.listCols[db;`trade]; colNames];
        dbm.addCols[db;`sym;`trade;`size`venue;0N];
        unit.assert.match[dbm.listCols[db;`trade]; colNames];

        dbm.addCols[db;`sym;`trade;`newColA`newColB;0N];
        {
            unit.assert.true all `newColA`newColB in key x;
            unit.assert.true all `newColA`newColB in get x,`.d;
            unit.assert.true all {all 0N=get x,y}[x;] each `newColA`newColB;
        } each tdirs;

        unit.assert.match[dbm.listCols[db;`trade]; colNames,`newColA`newColB];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testDelCol:{[]
    init[];

    check:{[db;tdirs]
        // No affect since column does not exist
        unit.assert.match[dbm.listCols[db;`trade]; colNames];
        dbm.delCol[db;`trade;`nonExistingCol];
        unit.assert.match[dbm.listCols[db;`trade]; colNames];

        dbm.delCol[db;`trade;`size];
        {
            unit.assert.false `size in key x;
            unit.assert.false `size in get x,`.d;
        } each tdirs;
        unit.assert.match[dbm.listCols[db;`trade]; colNames except `size];

        // Delete nested - should delete associated # file
        {unit.assert.true all (`company,`$"company#") in key x} each tdirs;
        dbm.delCol[db;`trade;`company];
        {unit.assert.false all (`company,`$"company#") in key x} each tdirs;
        unit.assert.match[dbm.listCols[db;`trade]; colNames except `size`company];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testCopyCol:{[]
    init[];

    check:{[db;tdirs]
        // No affect since price already exists
        unit.assert.match[dbm.listCols[db;`trade]; colNames];
        dbm.copyCol[db;`trade;`size;`price];
        unit.assert.match[dbm.listCols[db;`trade]; colNames];

        dbm.copyCol[db;`trade;`size;`sizeCopy];
        {
            unit.assert.true `sizeCopy in key x;
            unit.assert.true `sizeCopy in get x,`.d;
            unit.assert.match[get x,`size;get x,`sizeCopy];
        } each tdirs;
        unit.assert.match[dbm.listCols[db;`trade]; colNames,`sizeCopy];

        // Copy nested - should copy associated # file
        dbm.copyCol[db;`trade;`company;`companyCopy];
        {
            unit.assert.true all (`companyCopy,`$"companyCopy#") in key x;
            unit.assert.match[get x,`companyCopy;get x,`companyCopy];
            unit.assert.match[get x,`$"companyCopy#";get x,`$"companyCopy#"];
        } each tdirs;
        unit.assert.match[dbm.listCols[db;`trade]; colNames,`sizeCopy`companyCopy];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testHasCol:{[]
    init[];
    
    unit.assert.true dbm.hasCol[splayDB;`trade;`size];
    unit.assert.false dbm.hasCol[splayDB;`trade;`nonExistingCol];
    unit.assert.true dbm.hasCol[partDB;`trade;`size];
    unit.assert.false dbm.hasCol[partDB;`trade;`nonExistingCol];
 };

testListTabs:{[]
    init[];
    
    unit.assert.match[dbm.listTabs splayDB; enlist `trade];
    unit.assert.match[dbm.listTabs partDB; enlist `trade];

    schema:([] sym:`$(); ap:"f"$(); bp:"f"$());
    dbm.addTab[splayDB;`sym;`quote;schema];
    dbm.addTab[partDB;`sym;`quote;schema];

    unit.assert.match[dbm.listTabs splayDB; `quote`trade];
    unit.assert.match[dbm.listTabs partDB; `quote`trade];

    .Q.dd[partDB;`quote2`] set .Q.en[partDB;schema];
    unit.assert.match[dbm.listTabs partDB; `quote`quote2`trade];
 };

testRenameCol:{[]
    init[];

    check:{[db;tdirs]
        // No affect since price already exists
        unit.assert.match[dbm.listCols[db;`trade]; colNames];
        dbm.renameCol[db;`trade;`size;`price];
        unit.assert.match[dbm.listCols[db;`trade]; colNames];

        sizeData:get first[tdirs],`size;
        dbm.renameCol[db;`trade;`size;`sizeRenamed];
        {
            unit.assert.true `sizeRenamed in key x;
            unit.assert.true `sizeRenamed in get x,`.d;
            unit.assert.false `size in key x;
            unit.assert.false `size in get x,`.d;
            unit.assert.match[y;get x,`sizeRenamed];
        }[;sizeData] each tdirs;
        newColNames:@[colNames;where colNames=`size;:;`sizeRenamed];
        unit.assert.match[dbm.listCols[db;`trade]; newColNames];

        // Rename nested - should rename associated # file
        companyData:get first[tdirs],`company;
        companyHashData:get first[tdirs],`$"company#";
        dbm.renameCol[db;`trade;`company;`companyRenamed];
        {
            unit.assert.true all (`companyRenamed,`$"companyRenamed#") in key x;
            unit.assert.match[y;get x,`companyRenamed];
            unit.assert.match[z;get x,`$"companyRenamed#"];
        }[;companyData;companyHashData] each tdirs;
        newColNames:@[newColNames;where newColNames=`company;:;`companyRenamed];
        unit.assert.match[dbm.listCols[db;`trade]; newColNames];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testReorderCols:{[]
    init[];

    check:{[db]
        unit.assert.match[dbm.listCols[db;`trade]; colNames];

        unit.assert.fail[
            dbm.reorderCols;
            (db;`trade;colNames,`unknownCol);
            "Unknown column(s): unknownCol"
        ];

        dbm.reorderCols[db;`trade;reverse colNames];
        unit.assert.match[dbm.listCols[db;`trade]; reverse colNames];

        // Only named columns are reordered
        namedCols:`sym`company`time;
        dbm.reorderCols[db;`trade;namedCols];
        unit.assert.match[dbm.listCols[db;`trade]; namedCols,reverse[colNames] except namedCols];
    };

    check splayDB;
    check partDB;
 };

testFnCol:{[]
    init[];

    check:{[db;tdirs]
        // No affect since column does not exist
        dbm.fnCol[db;`trade;`nonExistingCol;10*];

        sizeData:get first[tdirs],`size;
        companyData:get first[tdirs],`company;

        dbm.fnCol[db;`trade;`size;10*];
        dbm.fnCol[db;`trade;`company;upper];
        {
            unit.assert.match[10*y;get x,`size];
            unit.assert.match[upper z;get x,`company];
        }[;sizeData;companyData] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testCastCol:{[]
    init[];

    check:{[db;tdirs]
        // No affect since column does not exist
        dbm.castCol[db;`trade;`nonExistingCol;10*];

        // Long to float
        {unit.assert.eq[7h;type get x,`size]} each tdirs;
        dbm.castCol[db;`trade;`size;"f"];
        {unit.assert.eq[9h;type get x,`size]} each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testAttr:{[]
    init[];

    check:{[db;tdirs]
        // No affect since column does not exist
        dbm.setAttr[db;`trade;`nonExistingCol;`s];

        {unit.assert.eq[`;attr get x,`size]} each tdirs;
        dbm.setAttr[db;`trade;`size;`s];
        {unit.assert.eq[`s;attr get x,`size]} each tdirs;

        dbm.rmAttr[db;`trade;`size];
        {unit.assert.eq[`;attr get x,`size]} each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testAddMissingCols:{[]
    init[];

    check:{[db;tdirs]
        goodTdir:`$(string first tdirs),"Copy";
        fs.rcopy[first tdirs;goodTdir];
        
        // Single missing column
        unit.assert.match[dbm.listCols[db;`trade]; colNames];
        dbm.delCol[db;`trade;`size];
        unit.assert.match[dbm.listCols[db;`trade]; colNames except `size];
        dbm.addMissingCols[db;`trade;goodTdir];
        unit.assert.match[dbm.listCols[db;`trade]; colNames];

        // Multiple missing columns (including nested)
        dbm.delCol[db;`trade;] each `size`price`company;
        unit.assert.match[dbm.listCols[db;`trade]; colNames except `size`price`company];
        dbm.addMissingCols[db;`trade;goodTdir];
        unit.assert.match[dbm.listCols[db;`trade]; colNames];
        {unit.assert.true all (`company,`$"company#") in key x} each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testAddTab:{[]
    init[];

    check:{[db;parts]
        schema:([] sym:`$(); ap:"f"$(); bp:"f"$());
        {unit.assert.false `quote in key .Q.dd[x;y]}[db;] each parts;
        
        dbm.addTab[db;`sym;`quote;schema];
        {unit.assert.true `quote in key .Q.dd[x;y]}[db;] each parts;
        unit.assert.match[dbm.listCols[db;`quote]; `sym`ap`bp];
    };

    check[splayDB;`];
    check[partDB;partDates];
 };

testDelTab:{[]
    init[];

    check:{[db;parts]
        {unit.assert.true `trade in key .Q.dd[x;y]}[db;] each parts;
        dbm.delTab[db;`trade];
        {unit.assert.false `trade in key .Q.dd[x;y]}[db;] each parts;
    };

    check[splayDB;`];
    check[partDB;partDates];
 };

testRenameTab:{[]
    init[];

    check:{[db;parts]
        {unit.assert.true `trade in key .Q.dd[x;y]}[db;] each parts;
        dbm.renameTab[db;`trade;`tradeRenamed];
        {unit.assert.false `trade in key .Q.dd[x;y]}[db;] each parts;
        {unit.assert.true `tradeRenamed in key .Q.dd[x;y]}[db;] each parts;
        unit.assert.match[dbm.listCols[db;`tradeRenamed]; colNames];
    };

    check[splayDB;`];
    check[partDB;partDates];
 };

testStrToSymCol:{[]
    init[];

    check:{[db;tdirs]
        companyData:get first[tdirs],`company;
        dbm.strToSymCol[db;`trade;`company;`sym];
        domain:get db,`sym;
        unit.assert.true all (`$companyData) in domain;
        {unit.assert.match[y;x get z,`company]}[domain;`$companyData;] each tdirs;

        // Works for char column too
        dbm.addCol[db;`;`trade;`charCol;"a"];
        {.Q.dd[y;`charCol] set count[x]#.Q.a}[companyData;] each tdirs;
        charData:get first[tdirs],`charCol;
        dbm.strToSymCol[db;`trade;`charCol;`sym];
        domain:get db,`sym;
        unit.assert.true all (`$/:charData) in domain;
        {unit.assert.match[y;x get z,`charCol]}[domain;`$/:charData;] each tdirs;

        // Failure for non-string column
        unit.assert.fail[dbm.strToSymCol; (db;`trade;`size;`sym); "type"];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testSymToStrCol:{[]
    init[];

    check:{[db;tdirs]
        // No affect since column is not string type
        sizeData:get first[tdirs],`size;
        dbm.symToStrCol[db;`trade;`size];
        {unit.assert.match[x;get y,`size]}[sizeData;] each tdirs;

        venueData:get[db,`sym] get first[tdirs],`venue;
        dbm.symToStrCol[db;`trade;`venue];
        {unit.assert.match[x;get y,`venue]}[string venueData;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testDomainUsed:{[]
    init[];

    check:{[db;tdirs]
        indexes:value (union) . (get first[tdirs],) each `sym`venue;
        unit.assert.match[indexes;dbm.domainUsed[db;`trade]`sym];

        dbm.addCol[db;`newSym;`trade;`newSymCol;`hello];
        delete newSym from `.;
        used:dbm.domainUsed[db;`trade];
        unit.assert.match[indexes;used`sym];
        unit.assert.match[enlist 0;used`newSym];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testDomainUnused:{[]
    init[];

    check:{[db;tdirs]
        unit.assert.match[`long$();dbm.domainUnused[db;`trade]`sym];
        
        indexes:value distinct get first[tdirs],`venue;
        dbm.delCol[db;`trade;`venue];
        unit.assert.match[indexes;dbm.domainUnused[db;`trade]`sym];

        dbm.addCol[db;`newSym;`trade;`newSymCol;`hello];
        delete newSym from `.;
        used:dbm.domainUnused[db;`trade];
        unit.assert.match[indexes;used`sym];
        unit.assert.match[`long$();used`newSym];

        dbm.delCol[db;`trade;`newSymCol];
        unit.assert.false `newSym in key dbm.domainUnused[db;`trade];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testDomainUsage:{[]
    init[];

    check:{[db;tdirs]
        indexes:value (union) . (get first[tdirs],) each `sym`venue;
        unit.assert.match[indexes;dbm.domainUsage[db]`sym];

        dbm.addCol[db;`newSym;`trade;`newSymCol;`hello];
        delete newSym from `.;
        used:dbm.domainUsage db;
        unit.assert.match[indexes;used`sym];
        unit.assert.match[enlist 0;used`newSym];

        // Add new table with new symbols
        dbm.addTab[db;`sym;`quote;([] sym:`$(); ap:"f"$(); bp:"f"$())];
        tdir:.Q.dd[fs.dirname first tdirs;`quote];
        tdir set .Q.en[db;([] sym:`one`two`three; ap:1 2 3f; bp:4 5 6f)];
        delete sym from `.;

        used:dbm.domainUsage db;
        unit.assert.match[indexes,value get tdir,`sym; asc used`sym];
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testResolveDomainMap:{[]
    init[];

    check:{[db]
        dm:dbm.domainUsage db;
        syms:get[db,`sym] dm;
        unit.assert.match[syms;dbm.resolveDomainMap[db;dm]];

        dbm.addCol[db;`newSym;`trade;`newSymCol;`hello];
        delete newSym from `.;
        dm:dbm.domainUsage db;
        syms:`sym`newSym!(get[db,`sym] dm`sym;get[db,`newSym] dm`newSym);
        unit.assert.match[syms;dbm.resolveDomainMap[db;dm]];
    };

    check splayDB;
    check partDB;
 };

testPersistDomainMap:{[]
    init[];

    check:{[db]
        dir:.Q.dd[tmp;`temp];
        
        dm:dbm.resolveDomainMap[db;] dbm.domainUsage db;
        paths:dbm.persistDomainMap[dir;dm];
        unit.assert.match[.Q.dd[dir;`sym];paths`sym];
        unit.assert.match[dm`sym;get paths`sym];

        dbm.addCol[db;`newSym;`trade;`newSymCol;`hello];
        delete newSym from `.;
        dm:dbm.resolveDomainMap[db;] dbm.domainUsage db;
        paths:dbm.persistDomainMap[dir;dm];
        unit.assert.match[`sym`newSym!.Q.dd[dir;] each `sym`newSym;paths];
        unit.assert.match[dm;get each paths];
    };

    check splayDB;
    check partDB;
 };

testRebuildDomains:{[]
    init[];

    check:{[db]
        dir:.Q.dd[tmp;`temp];

        paths:dbm.rebuildDomains[db;dir];
        dm:dbm.resolveDomainMap[db;] dbm.domainUsage db;
        unit.assert.match[.Q.dd[dir;`sym];paths`sym];
        unit.assert.match[dm`sym;get paths`sym];

        dbm.addCol[db;`newSym;`trade;`newSymCol;`hello];
        delete newSym from `.;
        paths:dbm.rebuildDomains[db;dir];
        dm:dbm.resolveDomainMap[db;] dbm.domainUsage db;
        unit.assert.match[`sym`newSym!.Q.dd[dir;] each `sym`newSym;paths];
        unit.assert.match[dm;get each paths];
    };

    check splayDB;
    check partDB;
 };

testReenumerateCol:{[]
    init[];

    check:{[db;tdirs]
        // sym column currently enumerated against sym
        data:get first[tdirs],`sym;
        unit.assert.eq[`sym;key data];
        data:get[db,`sym] data;

        // Create newSym
        newDomainFile:.Q.dd[db;`newSym];
        newDomainFile set reverse get db,`sym;

        // Re-enumerate sym column against newSym
        dbm.reenumerateCol[db;`trade;`sym;newDomainFile];

        // Check correctly re-enumerated
        {[db;data;tdir]
            newData:get tdir,`sym;
            unit.assert.eq[`newSym;key newData];
            unit.assert.match[data;(get db,`newSym) newData];
        }[db;data;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testReenumerateColFrom:{[]
    init[];

    check:{[db;tdirs]
        // sym column currently enumerated against sym
        data:get first[tdirs],`sym;
        unit.assert.eq[`sym;key data];
        data:get[db,`sym] data;

        // Create newSym
        newDomainFile:.Q.dd[db;`newSym];
        newDomainFile set reverse get db,`sym;

        // Try re-enumerate from symOther to newSym
        dbm.reenumerateColFrom[db;`trade;`sym;`symOther;newDomainFile];

        // Check no re-enumeration occured
        {[db;data;tdir]
            newData:get tdir,`sym;
            unit.assert.eq[`sym;key newData];
            unit.assert.match[data;(get db,`sym) newData];
        }[db;data;] each tdirs;

        // Re-enumerate sym column against newSym
        dbm.reenumerateColFrom[db;`trade;`sym;`sym;newDomainFile];

        // Check correctly re-enumerated
        {[db;data;tdir]
            newData:get tdir,`sym;
            unit.assert.eq[`newSym;key newData];
            unit.assert.match[data;(get db,`newSym) newData];
        }[db;data;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testReenumerateTab:{[]
    init[];

    check:{[db;tdirs]
        // sym & venue columns currently enumerated against sym
        symData:get first[tdirs],`sym;
        unit.assert.eq[`sym;key symData];
        symData:get[db,`sym] symData;
        
        venueData:get first[tdirs],`venue;
        unit.assert.eq[`sym;key venueData];
        venueData:get[db,`sym] venueData;

        // Create newSym
        newDomainFile:.Q.dd[db;`newSym];
        newDomainFile set reverse get db,`sym;

        // Re-enumerate against newSym
        dbm.reenumerateTab[db;`trade;newDomainFile];

        // Check correctly re-enumerated
        {[db;symData;venueData;tdir]
            newSymData:get tdir,`sym;
            unit.assert.eq[`newSym;key newSymData];
            unit.assert.match[symData;(get db,`newSym) newSymData];

            newVenueData:get tdir,`venue;
            unit.assert.eq[`newSym;key newVenueData];
            unit.assert.match[venueData;(get db,`newSym) newVenueData];
        }[db;symData;venueData;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testReenumerateTabFrom:{[]
    init[];

    check:{[db;tdirs]
        // sym & venue columns currently enumerated against sym
        symData:get first[tdirs],`sym;
        unit.assert.eq[`sym;key symData];
        symData:get[db,`sym] symData;
        
        venueData:get first[tdirs],`venue;
        unit.assert.eq[`sym;key venueData];
        venueData:get[db,`sym] venueData;

        // Create newSym
        newDomainFile:.Q.dd[db;`newSym];
        newDomainFile set reverse get db,`sym;

        // Try re-enumerate from symOther to newSym
        dbm.reenumerateTabFrom[db;`trade;`symOther;newDomainFile];

        // Check no re-enumeration occured
        {[db;symData;venueData;tdir]
            newSymData:get tdir,`sym;
            unit.assert.eq[`sym;key newSymData];
            unit.assert.match[symData;(get db,`sym) newSymData];

            newVenueData:get tdir,`venue;
            unit.assert.eq[`sym;key newVenueData];
            unit.assert.match[venueData;(get db,`sym) newVenueData];
        }[db;symData;venueData;] each tdirs;

        // Re-enumerate against newSym
        dbm.reenumerateTabFrom[db;`trade;`sym;newDomainFile];

        // Check correctly re-enumerated
        {[db;symData;venueData;tdir]
            newSymData:get tdir,`sym;
            unit.assert.eq[`newSym;key newSymData];
            unit.assert.match[symData;(get db,`newSym) newSymData];

            newVenueData:get tdir,`venue;
            unit.assert.eq[`newSym;key newVenueData];
            unit.assert.match[venueData;(get db,`newSym) newVenueData];
        }[db;symData;venueData;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testReenumerateAll:{[]
    init[];

    check:{[db;tdirs]
        // Create quote table 
        .z.m.quote:([]
            time:5#.z.P;
            sym:`IBM`AMZN`GOOGL`META`SPOT;
            ap:1 2 3 4 5f;
            bp:1 2 3 4 5f
        );
        .Q.dd[splayDB;`quote`] set .Q.en[splayDB;.z.m.quote];
        {.Q.dd[partDB;x,`quote,`] set .Q.en[partDB;.z.m.quote]} each partDates;
        delete sym, quote from `.;

        // sym column in trade and quote tables currently enumerated against sym
        tradeData:get first[tdirs],`sym;
        unit.assert.eq[`sym;key tradeData];
        tradeData:get[db,`sym] tradeData;
        
        quoteData:get (fs.dirname[first tdirs],`quote),`sym;
        unit.assert.eq[`sym;key quoteData];
        quoteData:get[db,`sym] quoteData;

        // Create newSym
        newDomainFile:.Q.dd[db;`newSym];
        newDomainFile set reverse get db,`sym;

        // Re-enumerate against newSym
        dbm.reenumerateAll[db;newDomainFile];

        // Check correctly re-enumerated
        {[db;tradeData;quoteData;tdir]
            newTradeData:get tdir,`sym;
            unit.assert.eq[`newSym;key newTradeData];
            unit.assert.match[tradeData;(get db,`newSym) newTradeData];

            newQuoteData:get (fs.dirname[tdir],`quote),`sym;
            unit.assert.eq[`newSym;key newQuoteData];
            unit.assert.match[quoteData;(get db,`newSym) newQuoteData];
        }[db;tradeData;quoteData;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testReenumerateAllFrom:{[]
    init[];

    check:{[db;tdirs]
        // Create quote table 
        quote::([]
            time:5#.z.P;
            sym:`IBM`AMZN`GOOGL`META`SPOT;
            ap:1 2 3 4 5f;
            bp:1 2 3 4 5f
        );
        .Q.dd[splayDB;`quote`] set .Q.en[splayDB;quote];
        {.Q.dd[partDB;x,`quote,`] set .Q.en[partDB;quote]} each partDates;
        delete sym, quote from `.;

        // sym column in trade and quote tables currently enumerated against sym
        tradeData:get first[tdirs],`sym;
        unit.assert.eq[`sym;key tradeData];
        tradeData:get[db,`sym] tradeData;
        
        quoteData:get (fs.dirname[first tdirs],`quote),`sym;
        unit.assert.eq[`sym;key quoteData];
        quoteData:get[db,`sym] quoteData;

        // Create newSym
        newDomainFile:.Q.dd[db;`newSym];
        newDomainFile set reverse get db,`sym;

        // Try re-enumerate from symOther to newSym
        dbm.reenumerateAllFrom[db;`symOther;newDomainFile];

        // Check no re-enumeration occured
        {[db;tradeData;quoteData;tdir]
            newTradeData:get tdir,`sym;
            unit.assert.eq[`sym;key newTradeData];
            unit.assert.match[tradeData;(get db,`sym) newTradeData];

            newQuoteData:get (fs.dirname[tdir],`quote),`sym;
            unit.assert.eq[`sym;key newQuoteData];
            unit.assert.match[quoteData;(get db,`sym) newQuoteData];
        }[db;tradeData;quoteData;] each tdirs;

        // Re-enumerate against newSym
        dbm.reenumerateAllFrom[db;`sym;newDomainFile];

        // Check correctly re-enumerated
        {[db;tradeData;quoteData;tdir]
            newTradeData:get tdir,`sym;
            unit.assert.eq[`newSym;key newTradeData];
            unit.assert.match[tradeData;(get db,`newSym) newTradeData];

            newQuoteData:get (fs.dirname[tdir],`quote),`sym;
            unit.assert.eq[`newSym;key newQuoteData];
            unit.assert.match[quoteData;(get db,`newSym) newQuoteData];
        }[db;tradeData;quoteData;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

testRenameDomain:{[]
    init[];

    check:{[db;tdirs]
        // Create quote table 
        .z.m.quote:([]
            time:5#.z.P;
            sym:`IBM`AMZN`GOOGL`META`SPOT;
            ap:1 2 3 4 5f;
            bp:1 2 3 4 5f
        );
        .Q.dd[splayDB;`quote`] set .Q.en[splayDB;.z.m.quote];
        {.Q.dd[partDB;x,`quote,`] set .Q.en[partDB;.z.m.quote]} each partDates;
        delete sym, quote from `.;

        // Create newSym
        newDomainFile:.Q.dd[db;`newSym];
        newDomainFile set reverse get db,`sym;

        // Re-enumerate sym column in trade only against newSym
        dbm.reenumerateCol[db;`trade;`sym;newDomainFile];

        // Check only sym in trade re-enumerated
        {[db;tdir]
            unit.assert.eq[`newSym;key get tdir,`sym];
            unit.assert.eq[`sym;key get tdir,`venue];
            unit.assert.eq[`sym;key get (fs.dirname[tdir],`quote),`sym];
        }[db;] each tdirs;

        // Rename newSym to newSymRenamed
        dbm.renameDomain[db;`newSym;`newSymRenamed];

        // Check only sym in trade had domain renamed
        {[db;tdir]
            unit.assert.eq[`newSymRenamed;key get tdir,`sym];
            unit.assert.eq[`sym;key get tdir,`venue];
            unit.assert.eq[`sym;key get (fs.dirname[tdir],`quote),`sym];
        }[db;] each tdirs;

        // Rename sym to symRenamed
        dbm.renameDomain[db;`sym;`symRenamed];

        // Check only venue in trade sym in quote  had domain renamed
        {[db;tdir]
            unit.assert.eq[`newSymRenamed;key get tdir,`sym];
            unit.assert.eq[`symRenamed;key get tdir,`venue];
            unit.assert.eq[`symRenamed;key get (fs.dirname[tdir],`quote),`sym];
        }[db;] each tdirs;
    };

    check[splayDB;splayTdir];
    check[partDB;partTdirs];
 };

export:([
    testListCols; testAddCol; testAddCols; testDelCol; testCopyCol; testHasCol; testListTabs; 
    testRenameCol; testReorderCols; testFnCol; testCastCol; testAttr; testAddMissingCols;
    testAddTab; testDelTab; testRenameTab; testStrToSymCol; testSymToStrCol; testDomainUsed;
    testDomainUnused; testDomainUsage; testResolveDomainMap; testPersistDomainMap;
    testRebuildDomains; testReenumerateCol; testReenumerateColFrom; testReenumerateTab;
    testReenumerateTabFrom; testReenumerateAll; testReenumerateAllFrom; testRenameDomain
 ]);
