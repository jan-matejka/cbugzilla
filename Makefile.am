ACLOCAL_AMFLAGS = -I m4

AM_LDFLAGS = -no-undefined

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = libcbugzilla.pc

EXTRA_DIST = \
	autogen.sh \
	libcbugzilla.pc.in \
	test/response/namedcmd.html \
	README.rst \
	ChangeLog.rst

libcbugzilla_includedir = $(includedir)/libcbugzilla
libcbugzilla_include_HEADERS = \
	libcbugzilla/cb.h \
	config.h

noinst_HEADERS = \
	libcbugzilla/bugzilla.h \
	libcbugzilla/bugzilla_parser.h \
	libcbugzilla/curl.h \
	libcbugzilla/_cb.h \
	libcbugzilla/htmltidy.h \
	libcbugzilla/string.h

bin_PROGRAMS = cbugzilla
cbugzilla_SOURCES = \
	src/main.c
# not adding tidy as it is added to CFLAGS/LIBS directly
cbugzilla_CFLAGS = \
	$(CURL_CFLAGS) \
	$(XDG_CFLAGS)
cbugzilla_LDADD = \
	$(CURL_LIBS) \
	$(XDG_LIBS) \
	libcbugzilla.la

lib_LTLIBRARIES = libcbugzilla.la
libcbugzilla_la_SOURCES = \
	src/bugzilla.c \
	src/bugzilla_parser.c \
	src/cb.c \
	src/curl.c \
	src/htmltidy.c \
	src/string.c
libcbugzilla_la_CFLAGS = \
	$(CURL_CFLAGS)
libcbugzilla_la_LDFLAGS = \
	$(CURL_LIBS)

check_PROGRAMS = parser string
parser_SOURCES = test/parser.c
parser_CFLAGS = \
	$(CURL_CFLAGS)
parser_LDADD = \
	$(CURL_LIBS) \
	libcbugzilla.la

string_SOURCES = test/string.c
string_CFLAGS = \
	$(CURL_CFLAGS)
string_LDADD = \
	$(CURL_LIBS) \
	libcbugzilla.la

TESTS = parser string

distclean-local:
	rm -rf *.cache *~

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

