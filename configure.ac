#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([linthesia], [0.4.2], [victor.lavaud@gmail.com])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

LT_PREREQ([2.2])
LT_INIT()

AC_ARG_WITH(
    [scriptdir],
    [AS_HELP_STRING([--with-scriptdir=<path to scripts>],[load scripts from this directory, default: $(bindir)])],
    [scriptdir="${with_scriptdir}"],
    [scriptdir="\$(bindir)"]
)
AC_SUBST([scriptdir])

AC_ARG_WITH(
    [graphdir],
    [AS_HELP_STRING([--with-graphdir=<path to images>],[load graphic resources from this directory, default: $(pkgdatadir)/graphics])],
    [graphdir="${with_graphdir}"],
    [graphdir="\$(pkgdatadir)/graphics"]
)
AC_SUBST([graphdir])

AC_ARG_WITH(
    [musicdir],
    [AS_HELP_STRING([--with-musicdir=<path to midis>],[load midi files from this directory, default: $(pkgdatadir)/music])],
    [musicdir="${with_musicdir}"],
    [musicdir="\$(pkgdatadir)/music"]
)
AC_SUBST([musicdir])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MKDIR_P

# Checks for header files.
AC_CHECK_HEADERS([string string.h iostream algorithm locale vector sstream queue functional map iomanip set exception sys/time.h])

PKG_CHECK_MODULES([GTKGLEXTMM],[gtkglextmm-1.2])
PKG_CHECK_MODULES([GTKMM],[gtkmm-2.4])
PKG_CHECK_MODULES([GCONFMM],[gconfmm-2.6])
PKG_CHECK_MODULES([FONTCONFIG],[fontconfig])
PKG_CHECK_MODULES([ALSA],[alsa])
PKG_CHECK_MODULES([LIBPANGO],[pango])
AC_CHECK_LIB(sqlite3, sqlite3_open,[echo "checking for sqlite3... yes"] , AC_MSG_ERROR([*** Unable to find sqlite3 library]), )

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday])

AC_CONFIG_FILES([Makefile src/Makefile scripts/Makefile])
AC_OUTPUT
