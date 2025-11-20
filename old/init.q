
/
    @file
        init.q
    
    @description
        Initialise QLib (main entry point). 
\

if[0=count key `.qlib.root;
    -2 "QLib requires '.qlib.root' to be set as the absolute path to the qlib directory";
    exit 1
 ];

.qlibi.path.build:.Q.dd[.qlib.root;`build];
.qlibi.path.bin:.Q.dd[.qlibi.path.build;`bin`lib];
.qlibi.path.config:.Q.dd[.qlib.root;`config];
.qlibi.path.src:.Q.dd[.qlib.root;`src];
.qlibi.path.test:.Q.dd[.qlib.root;`test];
.qlibi.path.unit:.Q.dd[.qlibi.path.test;`unit];

if[()~key .qlibi.path.build; system 1_string .Q.dd[.qlib.root;`build.sh]];
if[()~key .qlibi.path.build; '"Unable to create build"];
.qlibi.path.qlib:.qlibi.path.build;

.qlib.fstr.rep:.qlibi.path.bin 2: (`fstr_rep;1);
/ .qlib.fstr.format:.qlibi.path.bin 2: (`fstr_format;2);

/
// @brief Get the path to a config file.
// @param cnf Symbol File name.
// @return FileSymbol Path to a config file.
.pkg.internal.getCnfPath:{[cnf] .Q.dd[.pkg.internal.path.qlib;`cnf,cnf]};

// @brief Get a map from a config file.
// @param cnf Symbol Config name (without file extension which is assumed to be .json).
// @return Dict Map from config file.
.pkg.internal.getCnfMap:{[cnf] .j.k raze read0 .pkg.internal.getCnfPath ` sv cnf,`json};

// @brief Get a table from a config file.
// @param cnf Symbol Config name (without file extension which is assumed to be .csv).
// @param tys String Datatype characters for the columns of the table.
// @return Table Table from config file.
.pkg.internal.getCnfTable:{[cnf;tys] (tys;enlist csv) 0: .pkg.internal.getCnfPath ` sv cnf,`csv};
\

/ system "l ",1_string .Q.dd[.qlibi.path.qlib;`module.q];
