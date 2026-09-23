
/
    @file test_fstr.q
    @brief Unit tests for fstr.c module.
\

fstr:use`qlib.fstr;
unit:use`qlib.unit;

testFmtAtoms:{[]
    unit.assert.match["Hello";fstr.fmt["Hello";1]];

    // Boolean
    unit.assert.match["Hello 1b"; fstr.fmt["Hello {}";1b]];
    unit.assert.match["1b Hello"; fstr.fmt["{} Hello";1b]];
    unit.assert.match["Hel1blo";  fstr.fmt["Hel{}lo";1b]];

    // GUID
    unit.assert.match[
        "Hello 8c6b8b64-6815-6084-0a3e-178401251b68"; 
        fstr.fmt["Hello {}";"G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];
    unit.assert.match[
        "8c6b8b64-6815-6084-0a3e-178401251b68 Hello"; 
        fstr.fmt["{} Hello";"G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];
    unit.assert.match[
        "Hel8c6b8b64-6815-6084-0a3e-178401251b68lo";
        fstr.fmt["Hel{}lo";"G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];
    
    // Byte
    unit.assert.match["Hello 0x1f"; fstr.fmt["Hello {}";0x1f]];
    unit.assert.match["0x1f Hello"; fstr.fmt["{} Hello";0x1f]];
    unit.assert.match["Hel0x1flo";  fstr.fmt["Hel{}lo";0x1f]];
    unit.assert.match["Hello 0x0f"; fstr.fmt["Hello {}";0x0f]];

    // Short
    unit.assert.match["Hello 5"; fstr.fmt["Hello {}";5h]];
    unit.assert.match["5 Hello"; fstr.fmt["{} Hello";5h]];
    unit.assert.match["Hel5lo";  fstr.fmt["Hel{}lo";5h]];

    // Int
    unit.assert.match["Hello 5"; fstr.fmt["Hello {}";5i]];
    unit.assert.match["5 Hello"; fstr.fmt["{} Hello";5i]];
    unit.assert.match["Hel5lo";  fstr.fmt["Hel{}lo";5i]];

    // Long
    unit.assert.match["Hello 5"; fstr.fmt["Hello {}";5]];
    unit.assert.match["5 Hello"; fstr.fmt["{} Hello";5]];
    unit.assert.match["Hel5lo";  fstr.fmt["Hel{}lo";5]];

    // Real
    unit.assert.match["Hello 3.140000"; fstr.fmt["Hello {}";3.14e]];
    unit.assert.match["3.140000 Hello"; fstr.fmt["{} Hello";3.14e]];
    unit.assert.match["Hel3.140000lo";  fstr.fmt["Hel{}lo";3.14e]];
    // Defualt display rounding
    unit.assert.match["Hello 3.141593"; fstr.fmt["Hello {}";3.14159265e]]; 

    // Float
    unit.assert.match["Hello 3.140000"; fstr.fmt["Hello {}";3.14]];
    unit.assert.match["3.140000 Hello"; fstr.fmt["{} Hello";3.14]];
    unit.assert.match["Hel3.140000lo";  fstr.fmt["Hel{}lo";3.14]];
    // Defualt display rounding
    unit.assert.match["Hello 3.141593"; fstr.fmt["Hello {}";3.14159265]]; 

    // Char
    unit.assert.match["Hello q"; fstr.fmt["Hello {}";"q"]];
    unit.assert.match["q Hello"; fstr.fmt["{} Hello";"q"]];
    unit.assert.match["Helqlo";  fstr.fmt["Hel{}lo";"q"]];

    // Symbol
    unit.assert.match["Hello abc"; fstr.fmt["Hello {}";`abc]];
    unit.assert.match["abc Hello"; fstr.fmt["{} Hello";`abc]];
    unit.assert.match["Helabclo";  fstr.fmt["Hel{}lo";`abc]];

    // Timestamp
    unit.assert.match[
        "Hello 2026.01.17D03:56:10.123456789"; 
        fstr.fmt["Hello {}";2026.01.17D03:56:10.123456789]
    ];
    unit.assert.match[
        "2026.01.17D03:56:10.123456789 Hello";
        fstr.fmt["{} Hello";2026.01.17D03:56:10.123456789]
    ];
    unit.assert.match[
        "Hel2026.01.17D03:56:10.123456789lo";
        fstr.fmt["Hel{}lo";2026.01.17D03:56:10.123456789]
    ];

    // Month
    unit.assert.match["Hello 2026.04m"; fstr.fmt["Hello {}";2026.04m]];
    unit.assert.match["2026.04m Hello"; fstr.fmt["{} Hello";2026.04m]];
    unit.assert.match["Hel2026.04mlo";  fstr.fmt["Hel{}lo";2026.04m]];
    unit.assert.match["Hello 2021.11m"; fstr.fmt["Hello {}";2021.11m]];

    // Date
    unit.assert.match["Hello 2026.04.14"; fstr.fmt["Hello {}";2026.04.14]];
    unit.assert.match["2026.04.14 Hello"; fstr.fmt["{} Hello";2026.04.14]];
    unit.assert.match["Hel2026.04.14lo";  fstr.fmt["Hel{}lo";2026.04.14]];

    // Datetime
    unit.assert.match[
        "Hello 2026.04.14T08:16:18.011";
        fstr.fmt["Hello {}";2026.04.14T08:16:18.011]
    ];
    unit.assert.match[
        "2026.04.14T08:16:18.011 Hello";
        fstr.fmt["{} Hello";2026.04.14T08:16:18.011]
    ];
    unit.assert.match[
        "Hel2026.04.14T08:16:18.011lo";
        fstr.fmt["Hel{}lo";2026.04.14T08:16:18.011]
    ];

    // Timespan
    unit.assert.match[
        "Hello 3D10:54:38.123456789";
        fstr.fmt["Hello {}";3D10:54:38.123456789]
    ];
    unit.assert.match[
        "3D10:54:38.123456789 Hello";
        fstr.fmt["{} Hello";3D10:54:38.123456789]
    ];
    unit.assert.match[
        "Hel3D10:54:38.123456789lo";
        fstr.fmt["Hel{}lo";3D10:54:38.123456789]
    ];
    unit.assert.match[
        "Hello -3D10:54:38.123456789";
        fstr.fmt["Hello {}";-3D10:54:38.123456789]
    ];

    // Minute
    unit.assert.match["Hello 12:34";  fstr.fmt["Hello {}";12:34]];
    unit.assert.match["12:34 Hello";  fstr.fmt["{} Hello";12:34]];
    unit.assert.match["Hel12:34lo";   fstr.fmt["Hel{}lo";12:34]];
    unit.assert.match["Hello -12:34"; fstr.fmt["Hello {}";-12:34]];
    unit.assert.match["Hello 04:20";  fstr.fmt["Hello {}";04:20]];
    
    // Second
    unit.assert.match["Hello 12:34:56";  fstr.fmt["Hello {}";12:34:56]];
    unit.assert.match["12:34:56 Hello";  fstr.fmt["{} Hello";12:34:56]];
    unit.assert.match["Hel12:34:56lo";   fstr.fmt["Hel{}lo";12:34:56]];
    unit.assert.match["Hello -12:34:56"; fstr.fmt["Hello {}";-12:34:56]];
    unit.assert.match["Hello 04:20:00";  fstr.fmt["Hello {}";04:20:00]];
    
    // Time
    unit.assert.match["Hello 12:34:56.789";  fstr.fmt["Hello {}";12:34:56.789]];
    unit.assert.match["12:34:56.789 Hello";  fstr.fmt["{} Hello";12:34:56.789]];
    unit.assert.match["Hel12:34:56.789lo";   fstr.fmt["Hel{}lo";12:34:56.789]];
    unit.assert.match["Hello -12:34:56.789"; fstr.fmt["Hello {}";-12:34:56.789]];
    unit.assert.match["Hello 04:20:00.000";  fstr.fmt["Hello {}";04:20:00.000]];
 };

testFmtLists:{[]
    unit.assert.match["";fstr.fmt["";()]];
    unit.assert.match["Hello";fstr.fmt["Hello";()]];
    unit.assert.match["Hello";fstr.fmt["Hello";1 2 3]];

    // Boolean
    unit.assert.match["1b Hel0blo 1b"; fstr.fmt["{} Hel{}lo {}";101b]];
    unit.assert.match["Hello 1b"; fstr.fmt["Hello {}";enlist 1b]];
    unit.assert.match["Hello 101b"; fstr.fmt["Hello {}";enlist 101b]];
    unit.assert.match["101b Hello 101b"; fstr.fmt["{} Hello {}";2#enlist 101b]];

    // GUID
    unit.assert.match[
        " " sv (
            "8c6b8b64-6815-6084-0a3e-178401251b68";
            "Hel8c6b8b64-6815-6084-0a3e-178401251b68lo";
            "8c6b8b64-6815-6084-0a3e-178401251b68"
        ); 
        fstr.fmt["{} Hel{}lo {}";3#"G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];
    unit.assert.match[
        "Hello 8c6b8b64-6815-6084-0a3e-178401251b68";
        fstr.fmt["Hello {}";"G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];
    unit.assert.match[
        "Hello 8c6b8b64-6815-6084-0a3e-178401251b68";
        fstr.fmt["Hello {}";enlist "G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];
    unit.assert.match[
        " " sv enlist["Hello"],string 3#"G"$"8c6b8b64-6815-6084-0a3e-178401251b68";
        fstr.fmt["Hello {}";enlist 3#"G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];
    unit.assert.match[
        " " sv {x,enlist["Hello"],x} string 3#"G"$"8c6b8b64-6815-6084-0a3e-178401251b68";
        fstr.fmt["{} Hello {}";2#enlist 3#enlist "G"$"8c6b8b64-6815-6084-0a3e-178401251b68"]
    ];

    / fstr.fmt["{} Hel{}lo {}";101b]
    
    / // Byte
    / unit.assert.match["Hello 0x1f"; fstr.fmt["Hello {}";0x1f]];
    / unit.assert.match["0x1f Hello"; fstr.fmt["{} Hello";0x1f]];
    / unit.assert.match["Hel0x1flo";  fstr.fmt["Hel{}lo";0x1f]];
    / unit.assert.match["Hello 0x0f"; fstr.fmt["Hello {}";0x0f]];

    / // Short
    / unit.assert.match["Hello 5"; fstr.fmt["Hello {}";5h]];
    / unit.assert.match["5 Hello"; fstr.fmt["{} Hello";5h]];
    / unit.assert.match["Hel5lo";  fstr.fmt["Hel{}lo";5h]];

    / // Int
    / unit.assert.match["Hello 5"; fstr.fmt["Hello {}";5i]];
    / unit.assert.match["5 Hello"; fstr.fmt["{} Hello";5i]];
    / unit.assert.match["Hel5lo";  fstr.fmt["Hel{}lo";5i]];

    / // Long
    / unit.assert.match["Hello 5"; fstr.fmt["Hello {}";5]];
    / unit.assert.match["5 Hello"; fstr.fmt["{} Hello";5]];
    / unit.assert.match["Hel5lo";  fstr.fmt["Hel{}lo";5]];

    / // Real
    / unit.assert.match["Hello 3.140000"; fstr.fmt["Hello {}";3.14e]];
    / unit.assert.match["3.140000 Hello"; fstr.fmt["{} Hello";3.14e]];
    / unit.assert.match["Hel3.140000lo";  fstr.fmt["Hel{}lo";3.14e]];
    / // Defualt display rounding
    / unit.assert.match["Hello 3.141593"; fstr.fmt["Hello {}";3.14159265e]]; 

    / // Float
    / unit.assert.match["Hello 3.140000"; fstr.fmt["Hello {}";3.14]];
    / unit.assert.match["3.140000 Hello"; fstr.fmt["{} Hello";3.14]];
    / unit.assert.match["Hel3.140000lo";  fstr.fmt["Hel{}lo";3.14]];
    / // Defualt display rounding
    / unit.assert.match["Hello 3.141593"; fstr.fmt["Hello {}";3.14159265]]; 

    / // Char
    / unit.assert.match["Hello q"; fstr.fmt["Hello {}";"q"]];
    / unit.assert.match["q Hello"; fstr.fmt["{} Hello";"q"]];
    / unit.assert.match["Helqlo";  fstr.fmt["Hel{}lo";"q"]];

    / // Symbol
    / unit.assert.match["Hello abc"; fstr.fmt["Hello {}";`abc]];
    / unit.assert.match["abc Hello"; fstr.fmt["{} Hello";`abc]];
    / unit.assert.match["Helabclo";  fstr.fmt["Hel{}lo";`abc]];

    / // Timestamp
    / unit.assert.match[
    /     "Hello 2026.01.17D03:56:10.123456789"; 
    /     fstr.fmt["Hello {}";2026.01.17D03:56:10.123456789]
    / ];
    / unit.assert.match[
    /     "2026.01.17D03:56:10.123456789 Hello";
    /     fstr.fmt["{} Hello";2026.01.17D03:56:10.123456789]
    / ];
    / unit.assert.match[
    /     "Hel2026.01.17D03:56:10.123456789lo";
    /     fstr.fmt["Hel{}lo";2026.01.17D03:56:10.123456789]
    / ];

    / // Month
    / unit.assert.match["Hello 2026.04m"; fstr.fmt["Hello {}";2026.04m]];
    / unit.assert.match["2026.04m Hello"; fstr.fmt["{} Hello";2026.04m]];
    / unit.assert.match["Hel2026.04mlo";  fstr.fmt["Hel{}lo";2026.04m]];
    / unit.assert.match["Hello 2021.11m"; fstr.fmt["Hello {}";2021.11m]];

    / // Date
    / unit.assert.match["Hello 2026.04.14"; fstr.fmt["Hello {}";2026.04.14]];
    / unit.assert.match["2026.04.14 Hello"; fstr.fmt["{} Hello";2026.04.14]];
    / unit.assert.match["Hel2026.04.14lo";  fstr.fmt["Hel{}lo";2026.04.14]];

    / // Datetime
    / unit.assert.match[
    /     "Hello 2026.04.14T08:16:18.011";
    /     fstr.fmt["Hello {}";2026.04.14T08:16:18.011]
    / ];
    / unit.assert.match[
    /     "2026.04.14T08:16:18.011 Hello";
    /     fstr.fmt["{} Hello";2026.04.14T08:16:18.011]
    / ];
    / unit.assert.match[
    /     "Hel2026.04.14T08:16:18.011lo";
    /     fstr.fmt["Hel{}lo";2026.04.14T08:16:18.011]
    / ];

    / // Timespan
    / unit.assert.match[
    /     "Hello 3D10:54:38.123456789";
    /     fstr.fmt["Hello {}";3D10:54:38.123456789]
    / ];
    / unit.assert.match[
    /     "3D10:54:38.123456789 Hello";
    /     fstr.fmt["{} Hello";3D10:54:38.123456789]
    / ];
    / unit.assert.match[
    /     "Hel3D10:54:38.123456789lo";
    /     fstr.fmt["Hel{}lo";3D10:54:38.123456789]
    / ];
    / unit.assert.match[
    /     "Hello -3D10:54:38.123456789";
    /     fstr.fmt["Hello {}";-3D10:54:38.123456789]
    / ];

    / // Minute
    / unit.assert.match["Hello 12:34";  fstr.fmt["Hello {}";12:34]];
    / unit.assert.match["12:34 Hello";  fstr.fmt["{} Hello";12:34]];
    / unit.assert.match["Hel12:34lo";   fstr.fmt["Hel{}lo";12:34]];
    / unit.assert.match["Hello -12:34"; fstr.fmt["Hello {}";-12:34]];
    / unit.assert.match["Hello 04:20";  fstr.fmt["Hello {}";04:20]];
    
    / // Second
    / unit.assert.match["Hello 12:34:56";  fstr.fmt["Hello {}";12:34:56]];
    / unit.assert.match["12:34:56 Hello";  fstr.fmt["{} Hello";12:34:56]];
    / unit.assert.match["Hel12:34:56lo";   fstr.fmt["Hel{}lo";12:34:56]];
    / unit.assert.match["Hello -12:34:56"; fstr.fmt["Hello {}";-12:34:56]];
    / unit.assert.match["Hello 04:20:00";  fstr.fmt["Hello {}";04:20:00]];
    
    / // Time
    / unit.assert.match["Hello 12:34:56.789";  fstr.fmt["Hello {}";12:34:56.789]];
    / unit.assert.match["12:34:56.789 Hello";  fstr.fmt["{} Hello";12:34:56.789]];
    / unit.assert.match["Hel12:34:56.789lo";   fstr.fmt["Hel{}lo";12:34:56.789]];
    / unit.assert.match["Hello -12:34:56.789"; fstr.fmt["Hello {}";-12:34:56.789]];
    / unit.assert.match["Hello 04:20:00.000";  fstr.fmt["Hello {}";04:20:00.000]];
 };

testFmtErrors:{[]
    // First arg (format) must be a string
    unit.assert.fail[fstr.fmt;(`hello;());"type"];
    // Not enough format arguments provided
    unit.assert.fail[fstr.fmt;("Hello {} {}";1b);"length"];
 };

/
testFmtAtoms[];
testFmtLists[];
testFmtErrors[];
\

/ fstr.fmt["Hello {}";-04:20]

// TODO: Save args in variable and ensure still valid after

export:([testFmtAtoms; testFmtLists; testFmtErrors]);
