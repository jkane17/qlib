
/
    @file
        clap.q
    
    @description
        Command-Line Argument Parser.
\

// Table to hold command line arguments.
args:([name:1#`] 
    val:1#(); default:1#(); required:1#0b; given:1#0b; validate:1#(::); description:1#()
 );

// @brief Retrieve the value of an argument..
// @param name symbol Argument name.
// @return any The parsed value of the argument, or () if it does not exist..
.z.m.get:{[name:`s] args[name;`val]};

// @brief Check whether an argument was supplied on the command line..
// @param name symbol Argument name.
// @return bool 1b if the argument was supplied, 0b otherwise.
given:{[name:`s] args[name;`given]};

// @brief Register a command-line argument for parsing.
// @param name symbol Argument name.
// @param default any Default value. This also defines the argument’s type. Even if an argument is 
// required, provide a null of the correct type.
// @param required bool Whether the argument is mandatory.
// @param validate function A validation function applied to the parsed value. It must return a 
// boolean. Use (::) if no validation is needed.
// @param description string Human-readable description of the argument.
register:{[name:`s;default;required:`b;validate;description:`C]
    args::args upsert ([name;default;required;validate;description]);
 };

// @brief Return raw command-line arguments before any parsing or type conversion.
// @return dict Name → raw string value.
raw:{[] .Q.opt .z.X};

// @brief Parse command line arguments.
.z.m.parse:{[]
    cmdline:raw[];
    upd:flip `name`val!(key;value)@\:exec .Q.def[name!default;cmdline] from args;
    upd:upd lj ([name:key cmdline] given:count[cmdline]#1b);
    args::args upsert upd;
    if[any invalid:exec name!not validate@'val from args where not null validate;
        '"Invalid argument(s): ",", " sv string where invalid
    ];
    if[any missing:exec name!required and not given from args;
        '"Missing argument(s): ",", " sv string where missing
    ];
 };

// @brief Generate a usage/help table showing all registered arguments.
// @return string A formatted table describing arguments, defaults, and requirements.
usage:{[] .Q.s2 select name, default, required, description from args where not null name};

export:([.z.m.get; given; .z.m.parse; raw; register; usage]);
