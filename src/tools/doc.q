
/
    Generate code documentation from source files
\

PREFIX:"// @";
TAG_ORDER:`brief`param`return;
IGNORE_FNAMES:("*.priv.*";"*.internal.*");

split:" " vs count[PREFIX]_;
extractTag:`$first split@;
dropTag:" " sv 1_split@;

wrap:{"|",x,"|"};

makeMDTable:{[headings;rows]
    t:enlist wrap "|" sv string headings;
    t,:enlist wrap "|" sv count[headings]#enlist" - ";
    t,:(wrap"|" sv) each rows;
    t
 };

.parse.brief:{
    title:"### Brief";
    body:"\n" sv x;
    (title;body)
 };

.parse.param:{
    title:"### Parameters";
    rows:trim ":" vs/:x;
    rows[;1]:{@[x;where x="|";:;","]} each rows[;1];
    rows[;2]:rows[;2] except\: "|";
    body:makeMDTable[`Param`Type`Description;rows];
    enlist[title],body
 };

.parse.return:{
    title:"### Return";
    x:first x; // Only use the first @return
    body:("* Type : ";"* Description : "),'trim ":" vs x;
    enlist[title],body
 };

findDocLines:{[contents] where contents like PREFIX,"*"};
groupDocLines:{[i] cut[;i] 0,where 1<deltas[first i;i]};
extractFnames:{[contents;i] 
    (first ":" vs) each contents 1+last each i
 };
filterIgnoreLines:{[fnames] 
    where not any fnames like/: IGNORE_FNAMES
 };

genFuncDoc:{[fname;comment]
    title:"## ",fname;
    tags:group extractTag each comment;
    comment:dropTag each comment;
    tags:(TAG_ORDER inter key tags)#tags;
    tags:.parse[key tags]@'comment tags;
    (enlist[title],raze value tags),("";"<br />";"")
 };

generate:{[src;out]
    contents:read0 src;
    // TODO - Add file description to doc
    i:groupDocLines findDocLines contents;
    fnames:extractFnames[contents;i];
    comments:contents i fi:filterIgnoreLines fnames;
    fnames@:fi;
    out 0: raze genFuncDoc'[fnames;comments];
 };

/ src:`$":src/lib/log.q";
/ out:`$":docs/lib/log.md";

main:{[]
    generate[`$":src/lib/log.q";`$":docs/lib/log.md"];
    // TODO - Make this runnable from cmdline
 };

main[];
