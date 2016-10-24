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

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MKDIR_P

# Checks for header files.
AC_CHECK_HEADERS([string string.h iostream algorithm locale vector sstream queue functional map iomanip set exception sys/time.h])

PKG_CHECK_MODULES([GTKGLEXTMM],[gtkglextmm-1.2])
PKG_CHECK_MODULES([GTKMM],[gtkmm-2.4])
PKG_CHECK_MODULES([GCONFMM],[gconfmm-2.6])
PKG_CHECK_MODULES([ALSA],[alsa])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday])

AC_CONFIG_FILES([Makefile src/Makefile scripts/Makefile])
AC_OUTPUT
