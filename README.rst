=========
CBugzilla
=========

CLI and C api to get data from `Bugzilla <http://www.bugzilla.org/>`_.

Written specificaly for `Gentoo bugzilla <https://bugs.gentoo.org/>`_ but (in
theory) should work with any bugzilla (patches may be needed for different
versions or deploy with extensions).

The primary purpose of this tool is querying data for bug "Queue" length monitoring and graphing.

Usage
======

Set your authentication data in ~/.config/cbugzilla/auth (see cbugzilla -h for the format). And then you can just run

::

    cbugzilla <YourSavedQueryName>

And you get the number of records on STDOUT.

Alternately, you can link against it and use CGB_t and related functions to call the query and get the number.
