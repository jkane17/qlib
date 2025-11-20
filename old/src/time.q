
// Get day of week name via: days date mod 7
days:`sat`sun`mon`tue`wed`thu`fri;

// Return the first day (Monday) of the week of the given date
.date.weekStart:{[dt] `week$dt};

// Return the last day (Sunday) of the week of the given date
.date.weekEnd:{[dt] 6+`week$dt};

// Return the first day of the month of the given date
.date.monthStart:{[dt] `date$`month$dt};

// Return the last day of the month of the given date
.date.monthEnd:{[dt] -1+`date$1+`month$dt};

// Return the first day of the year of the given date
.date.yearStart:{[dt] "D"$string[`year$dt],".01.01"};

// Return the last day of the year of the given date
.date.yearEnd:{[dt] "D"$string[`year$dt],".12.31"};

// Return the most recent weekday of the given date
.date.prevWeekday:{[dt] dt-m*3>m:1+dt mod 7};

// Return a list of n previous weekday dates from the given date (inclusive)
.date.weekdays:{[dt;n] n sublist d where 1<mod[;7] d:dt-til 7*1+n div 5};

// Format a date
// Format specifiers: `d = Day (dd), `m = Month (mm), `y = Year (yyyy), `y2 = Year (yy)
.date.format:{[dt;fmt;dlm]
    d:"0"^-2$k!string $[;dt] (`d`m!`dd`mm) k:fmt except `y`y2;
    d,:(fmt inter`y`y2)!enlist $[`y2 in fmt;-2$string@;string] `year$dt;
    dlm sv d fmt
 };

// Convert a Unix Epoch timestamp (second-based) to a Q timestamp
.time.unixToQ:{[secs] "p"$"j"$(secs*0D00:00:01)-2000.01.01D-1970.01.01D};

// Convert a Q timestamp to a Unix Epoch timestamp (second-based)
.time.qToUnix:{[ts] "j"$(ts-1970.01.01D) div 0D00:00:00.001};

// Convert a Unix Epoch timestamp (millisecond-based) to a Q timestamp
.time.unixToQMS:{[ms] 1970.01.01+0D00:00:00.001*ms};

// Convert a Q timestamp to Unix Epoch timestamp (millisecond-based)
.time.qToUnixMS:{[ts] "j"$(ts-1970.01.01D)%0D00:00:00.001};
