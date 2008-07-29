AC_PREREQ(2.53)
AC_INIT(libframeworkd-glib, 0.1, http://www.openmoko.org/)
AM_INIT_AUTOMAKE()
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_GNU_SOURCE
AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

dnl glib-genmarshal
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

# base deps
PKG_CHECK_MODULES(DIALER,
                  dbus-glib-1)

AC_SUBST(DIALER_CFLAGS)
AC_SUBST(DIALER_LIBS)

# output stuff
AC_OUTPUT([
Makefile
src/Makefile
])
