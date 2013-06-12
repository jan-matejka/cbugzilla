0.2.0
=====

* cbugzilla binary no longer prints "Records for python-herd: " before the
number

* The code was completely reorganized

    * variable and function names make more sense now.

    * naming is more consistent.

    * The public API now consists only of file `libcbugzilla/cb.h` which
    provides `cbi_t` and `cbi_new` for the dependers.

* default value for CURLOPT_SSL_VERIFYHOST is now 2

* improved error handling

    * library return values now use CB_SUCCESS or CB_E<X> instead of
    EXIT_SUCCESS and EXIT_FAILURE

    * many missing return value checks were added

    * no more writes to stdout/stderr from library code. Error code of curl
    cals is provided by `cbi->get_curl_code`, the rest is TODO

0.1.1
=====

* The library now url escapes `namedcmd` and so can handle more than [a-z0-9]
characters.

* Unfortunately `cbugzilla -V` libcbugzilla.pc reports itself as 0.1.0

0.1.0
=====

Initial release.

The built cbugzilla binary can query number of records for given `namedcmd`.
