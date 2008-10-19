AC_PREREQ(2.53)
AC_INIT(libframeworkd-phonegui, 0.1, http://www.openmoko.org/)
AM_INIT_AUTOMAKE()
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_GNU_SOURCE
AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(PHONEGUI, libframeworkd-glib)
PKG_CHECK_MODULES(GLIB, glib-2.0)
AC_OUTPUT([
Makefile
data/Makefile
src/Makefile
libframeworkd-phonegui.pc
])

