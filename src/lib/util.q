
/
    Utility Functions
\

// @brief Get parse tree form.
// @param x : Any : Object to convert to parse tree.
// @return General List : Parse tree.
.util.ptree:{$[type x;parse x;-11=type f:first x;get[f],1_x;x]};
