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
PKG_CHECK_MODULES(FRAMEWORKD_GLIB, libframeworkd-glib)
PKG_CHECK_MODULES(DBUS_GLIB, dbus-glib-1)
PKG_CHECK_MODULES(GLIB, glib-2.0)

EXPAND(SYSCONFDIR, $sysconfdir)

AC_OUTPUT([
Makefile
data/Makefile
src/Makefile
src/frameworkd-phonegui.h
libframeworkd-phonegui.pc
])

