
/
    File:
        flt.q
    
    Description:
        Filtering utilities.
\

// @brief Keep x that has a count > 0.
// @param x : Lists : Values to filter.
// @return Lists : Values of x that have count > 0.
.flt.hasCount:{x where 0<count each x};

// @brief Keep x that matches pattern y.
// @param x Strings|Symbols Values to filter.
// @param y String Pattern to match.
// @return Strings|Symbols Values in x that match y.
.flt.like:{x where x like y};

// @brief Keep x that are not null.
// @param x : List : Values to filter.
// @return List : Values of x that are not null.
.flt.notNull:{x where not null x};
