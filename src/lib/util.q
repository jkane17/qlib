
/
    File:
        util.q
    
    Description:
        Utilities.
\

// @brief Generate a unique file name suffix.
// @return String : File name suffix.
.util.genFileSuffix:{[] string[.z.i],string[.z.p] except ".:D"};

// @brief Get parse tree form.
// @param x Any Object to convert to parse tree.
// @return GeneralList Parse tree.
.util.ptree:{$[type x;parse x;-11=type f:first x;get[f],1_x;x]};
