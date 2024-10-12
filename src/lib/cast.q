
/
    Casting from one type to another
\

// Cast a file symbol to a string (removes prevaling ":")
// h - FileSymbol - File symbol to cast.
.cast.htostr:{[h] $[":"=first h:.cast.tostr h;1_h;h]};

// Cast a value to a string
// x - Any - Value to cast
.cast.tostr:{$[10h=type x;x;string x]};

// Cast a value to a symbol
// x - Any - Value to cast
.cast.tosym:`$.cast.tostr@;

.cast.tohsym:hsym `$.cast.tostr@;


