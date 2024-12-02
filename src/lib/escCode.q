
/
    File:
        escCode.q
    
    Description:
        Terminal escape code handling.
\

.pkg.load `flt;

.escCode.priv.esc:"\033";
.escCode.priv.codes:.pkg.internal.getCnfMap[`escCode;"sh"];

// @brief Build a literal escape code from the given codes.
// @param codes Symbols Escape codes.
// @return String Literal escape code.
.escCode.priv.build:{[codes]
    codes:";" sv string .escCode.priv.codes (),codes;
    .escCode.priv.esc,"[",codes,"m"
 };

// @brief Apply escape codes to the given text.
// @param codes Symbols Escape codes to apply.
// @param text String Text to apply escape codes to.
// @return String Text wrapped with escape codes.
.escCode.apply:{[codes;text] .escCode.priv.build[codes],text,.escCode.priv.build`RESET};

// @brief List all supported escape codes.
// @return Symbols Escape codes.
.escCode.listAll:{[] key .escCode.priv.codes};

// @brief List foreground colors.
// @return Symbols Foreground colors.
.escCode.listFGColors:{[] .flt.like[.escCode.listAll[];"FG_*"]};

// @brief List background colors.
// @return Symbols Background colors.
.escCode.listBGColors:{[] .flt.like[.escCode.listAll[];"BG_*"]};

// @brief List all colors.
// @return Symbols All colors.
.escCode.listColors:{[] .escCode.listFGColors[],.escCode.listBGColors[]};
