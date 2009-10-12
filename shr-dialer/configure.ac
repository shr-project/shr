AC_PREREQ(2.53)
AC_INIT(shr-dialer, 3.0, http://shr-project.org/)
AM_INIT_AUTOMAKE()
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_GNU_SOURCE
AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL


# base deps
PKG_CHECK_MODULES(DBUS, dbus-glib-1)
                                   

AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)


# output stuff
AC_CONFIG_FILES([
Makefile
data/Makefile
src/Makefile
])

AC_OUTPUT
