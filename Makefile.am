ACLOCAL_AMFLAGS = -I m4

pkgconfdir = $(libdir)/pkgconfig

pkgconf_DATA = cbugzilla.pc

EXTRA_DIST = \
	autogen.sh \
	cbugzilla.pc.in
	test/response/namedcmd.html

bin_PROGRAMS = cbugzilla
cbugzilla_SOURCES = \
	src/main.c
# not adding tidy as it is added to CFLAGS/LIBS directly
cbugzilla_CFLAGS = \
	$(CURL_CFLAGS)
cbugzilla_LDADD = \
	$(CURL_LIBS)
	cbugzilla.la

lib_LTLIBRARIES = libcbugzilla.la
libcbugzilla_la_SOURCES = \
	src/bugzilla.c \
	src/bugzilla_parser.c \
	src/cgb.c \
	src/curl.c \
	src/htmltidy.c \
	src/string.c
libcbugzilla_la_CFLAGS = \
	$(CURL_CFLAGS)
libcbugzilla_la_LDFLAGS = \
	$(CURL_LIBS)

check_PROGRAMS = parser
parser_SOURCES = test/parser.c
parser_CFLAGS = \
	$(CURL_CFLAGS)
parser_LDADD = \
	$(CURL_LIBS)
	cbugzilla.la

TESTS = parser

distclean-local:
	rm -rf *.cache *~

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

