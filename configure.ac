# Process this file with autoconf to create configure.

AC_PREREQ([2.65])

# ====================
# Version informations
# ====================
m4_define([cbugzilla_version_major],[0])
m4_define([cbugzilla_version_minor],[2])
m4_define([cbugzilla_version_micro],[1])
m4_define([cbugzilla_version],[cbugzilla_version_major.cbugzilla_version_minor.cbugzilla_version_micro])

# =============
# Automake init
# =============
AC_INIT([cbugzilla],[cbugzilla_version])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])
AC_LANG([C])

# ===========================
# Find required base packages
# ===========================
LT_INIT([disable-static pic-only])
AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG([0.20])

# ====================
# Find additional apps
# ====================
PKG_CHECK_MODULES([CURL],[
	libcurl
])
PKG_CHECK_MODULES([XDG],[
	libxdg-basedir
])

# =================================
# Hand written checker for htmltidy
# =================================
AC_CHECK_HEADERS([buffio.h tidy.h], [], [
	AC_MSG_ERROR([Unable to find htmltidy headers])
])
AC_SEARCH_LIBS([tidyParseBuffer], [tidy], [], [
	AC_MSG_ERROR([Unable to find htmltidy library])
])

# ================
# Check for cflags
# ================
AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--disable-werror], [Treat all warnings as errors, useful for development])],
	[enable_werror="$enableval"],
	[enable_werror=yes]
)
AS_IF([test x"$enable_werror" != "xno"], [
	CFLAGS="$CFLAGS -Werror"
])
CFLAGS="$CFLAGS -Wall -Wextra -pedantic"

# =====================
# Prepare all .in files
# =====================
AC_CONFIG_FILES([
	Makefile
	libcbugzilla.pc
])
AC_OUTPUT

# ==============================================
# Display final informations about configuration
# ==============================================
AC_MSG_NOTICE([
==============================================================================
Build configuration:
	werror:          ${enable_werror}
==============================================================================
])
