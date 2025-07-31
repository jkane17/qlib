
/
    @file
        init.q
    
    @description
        Initialise QLib (main entry point). 
\

.qlibi.path.build:.Q.dd[.qlibi.path.root;`build];
.qlibi.path.config:.Q.dd[.qlibi.path.root;`config];
.qlibi.path.src:.Q.dd[.qlibi.path.root;`src];
.qlibi.path.test:.Q.dd[.qlibi.path.root;`test];
.qlibi.path.unit:.Q.dd[.qlibi.path.test;`unit];

.qlibi.path.qlib:$[
    not ()~key .qlibi.path.build; .Q.dd[.qlibi.path.build;`lib];
    not ()~key .qlibi.path.src; .qlibi.path.src;
    '"Missing build and src directories"
 ];

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

system "l ",1_string .Q.dd[.qlibi.path.qlib;`module.q];
