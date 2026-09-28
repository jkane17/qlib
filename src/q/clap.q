
/
    @file clap.q
    
    @overview
        A lightweight Q/KDB-X module for parsing command-line arguments in a structured and 
        type-safe way.

        Arguments must first be registered so they can be validated, defaulted, and accessed in a 
        consistent format.

        Parsing can then occur at any point before the arguments are used.
\

// Table to hold command line arguments.
args:([name:1#`] 
    val:1#(); default:1#(); required:1#0b; given:1#0b; validate:1#(::); description:1#()
 );

// @brief Retrieve the value of an argument.
// @param name {symbol} Argument name.
// @return {any} The parsed value of the argument, or `()` if it does not exist.
// @example
//     q)clap.get `myArg
//     10
.z.m.get:{[name:`s] args[name;`val]};

// @brief Check whether an argument was supplied on the command line..
// @param name {symbol} Argument name.
// @return {boolean} `1b` if the argument was supplied, `0b` otherwise.
// @example
//     q)clap.given `myArg
//     1b
given:{[name:`s] args[name;`given]};

// @brief Register a command-line argument for parsing.
// @param name {symbol} Argument name.
// @param default {any} Default value. This also defines the argument’s type. Even if an argument is 
// required, provide a null of the correct type.
// @param required {boolean} Whether the argument is mandatory.
// @param validate {function} A validation function applied to the parsed value. It must return a 
// boolean. Use (::) if no validation is needed.
// @param description {string} Human-readable description of the argument.
// @example
//     q)clap.register[`myLongArg; 10; 0b; 0<=; "Must be ≥ 0"]
//     q)clap.register[`myStringArg; enlist "Nothing"; 0b; (::); "String argument"]
//     q)clap.register[`myRequiredArg; 0nf; 1b; (::); "Required float argument"]
register:{[name:`s;default;required:`b;validate;description:`C]
    args::args upsert ([name;default;required;validate;description]);
 };

// @brief Return raw command-line arguments before any parsing or type conversion.
// @return {dict} Name Raw string value.
// @example
//     q)clap.raw[]
//     p     | "5000" 
//     myArg1| "10"   
//     myArg2| "hello"
raw:{[] .Q.opt .z.X};

// @brief Parse command line arguments.
// @example
//     q)clap.parse[]  // Should return no error if successful
// @example
//     q)clap.parse[]  // Invalid argument
//     'Invalid argument(s): myLongArg
//       [0]  clap.parse[]
// @example
//     q)clap.parse[]  // Missing required argument
//     'Missing argument(s): myRequiredArg
//       [0]  clap.parse[]
//            ^   
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
// @return {string} A formatted table describing arguments, defaults, and requirements.
// @example
//     q)-1 clap.usage[]
//     name          default    required description                                                   
//     -----------------------------------------------------------------------
//     myLongArg     10         0        "My long argument which must be >= 0"
//     myStringArg   ,"Nothing" 0        "My string argument"                                          
//     myRequiredArg 0n         1        "My required argument"  
usage:{[] select name, default, required, description from args where not null name};

export:([.z.m.get; given; .z.m.parse; raw; register; usage]);
