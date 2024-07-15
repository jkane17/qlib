
/ 
    Command Line Parsing
\

.pkg.load `fstr`log;

.argv.priv.args:([name:1#`] 
    ty:1#""; val:1#(); default:1#(); given:1#0b; 
    required:1#0b; vf:1#(); description:1#()
 );

// @brief Initialise argv.
.argv.priv.init:{[]
    .argv.add[`b;   "b"; 0b;    0b; {1b};                "Blocked"];
    .argv.add[`p;   "i"; 0i;    0b; {1b};                "Q Process Port"];
    .argv.add[`q;   "b"; 0b;    0b; {1b};                "Quiet Mode"];
    .argv.add[`s;   "i"; 0i;    0b; {1b};                "Secondary Processes"];
    .argv.add[`w;   "j"; 0;     0b; {1b};                "Q Memory Limit"];
    .argv.add[`T;   "j"; 0;     0b; {1b};                "Timeout"];
    .argv.add[`lvl; "s"; `INFO; 0b; .log.internal.valid; "Log Level"];
 };

// @brief Parse default values.
.argv.priv.parseDefaults:{[]
    update enlist each default from `.argv.priv.args 
        where 10h=type each default;
 }; 

// @brief Set default value of args that were not provided.
.argv.priv.fillDefaults:{[]
    .argv.priv.parseDefaults[];
    `.argv.priv.args upsert flip `name`val!(key;value)@\:
        exec .Q.def[name!default;.Q.opt .z.X] from .argv.priv.args;
    update first val from `.argv.priv.args where 10h=(type first@) each default;
    .argv.priv.args:update val:default from .argv.priv.args 
        where (all null@) each val;
 };

// @brief Update given flag to 1b if argument was provided, 0b otherwise.
.argv.priv.updGiven:{[]
    .argv.priv.args:update given:1b from .argv.priv.args 
        where name in key .Q.opt .z.X;
    .argv.priv.args:update val:given from .argv.priv.args where ty="b";
 };

// @brief Set values within any packages whose depend on command line arguments.
.argv.priv.setPkgVals:{[] 
    if[.pkg.loaded `log; .log.setLvl .argv.getValue`lvl];
 };

// @brief Validate command line arguments.
.argv.priv.validate:{[]
    valid:exec vf@'val from .argv.priv.args where not null name;
    if[not all valid;
        names:exec name from .argv.priv.args where not null name;
        .log.fatal .fstr.fmt[
            "Invalid command line argument(s): {}";
            names where not valid
        ];
        exit 1
    ]
 };

// @brief Log usage information.
.argv.priv.usage:{[]
    .log.debug (
        "Usage ";
        select ("-",/:string name), ty, required, description 
            from .argv.priv.args where not null name
    );
 };

// @brief Check we have all the required arguments.
.argv.priv.haveReq:{[]
    if[count req:exec name from .argv.priv.args where required, not given;
        .log.fatal .fstr.fmt[
            "Missing required command line argument(s): {}";
            req
        ];
        .argv.priv.usage[];
        exit 1
    ]
 };

// @brief Add a command line argument to be parsed.
// @param name : Symbol : Argument name.
// @param ty   : Char   : Argument type.
// @param def  : Any    : Argument default value.
// @param req  : Bool   : Is argument required?
// @param vf   : Lambda|Projection|Composition : (v)alidation (f)unction that 
// is applied to the arg value and should return a bool (pass or fail).
// @param des  : String : Description of argument.
.argv.add:{[name;ty;def;req;vf;des]
    `.argv.priv.args upsert `name`ty`default`required`vf`description!(
        name;ty;def;req;vf;des
    );
 };

// @brief Get value of argument.
// @param name : Symbol : Argument name.
// @return Any : Value of argument
.argv.getValue:{[name] .argv.priv.args[name;`val]};

// @brief Was the argument provided on the command line?
// @param name : Symbol : Argument name.
// @return Bool : 1b if argument was given, 0b otherwise.
.argv.given:{[name] .argv.priv.args[name;`given]};

// @brief Parse command line arguments.
.argv.parseCmdLine:{[] 
    .argv.priv.fillDefaults[];
    .argv.priv.updGiven[];
    .argv.priv.setPkgVals[];
    .argv.priv.validate[];
    .argv.priv.haveReq[];
 };

.argv.priv.init[];
