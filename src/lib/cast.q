
/
    File:
        cast.q
    
    Description:
        Type casting utilities.
\

// @brief Cast a file symbol to a string (removes prevaling ":").
// @param h FileSymbol File symbol to cast.
// @return String File symbol as a string.
.cast.htostr:{[h] $[":"=first h:.cast.tostr h;1_h;h]};

// @brief Cast a value to a string.
// @param x Any Value to cast.
// @return String Value as a string.
.cast.tostr:{$[10h=type x;x;string x]};

// @brief Cast a value to a symbol.
// @param x Any Value to cast.
// @return Symbol Value as a symbol.
.cast.tosym:`$.cast.tostr@;

// @brief Cast a value to a file symbol.
// @param x Any Value to cast.
// @return FileSymbol Value as a file symbol.
.cast.tohsym:hsym `$.cast.tostr@;
