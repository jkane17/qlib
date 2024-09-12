
/
    HTTP(S) requests
\

// @brief Convert a response header into a dictionary.
// @param h : String : Raw response header.
// @return Dict : Argument name.
.http.priv.formatHeader:{[h]
    h:"\r\n" vs h;
    h@:where 0<count each h;
    d:enlist[`status]!enlist first h;
    d,:(!). flip {(`$;ltrim 1_)@'(0,x?":") cut x} each 1_h;
    d
 };

// @brief Perform a GET request to the given URL.
// @param url : String | FileSymbol : URL of GET request.
// @return Dict : Two element dictionary of response formatted header and content.
.http.get:{[url] @[;`header;.http.priv.formatHeader] `header`content!.Q.hmb[url;`GET;()]};
