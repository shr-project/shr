AC_PREREQ(2.53)
AC_INIT(libframeworkd-phonegui-efl, 0.1, http://www.openmoko.org/)
AM_INIT_AUTOMAKE()
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_GNU_SOURCE
AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

# base deps
PKG_CHECK_MODULES(LIBFRAMEWORKD_GLIB, libframeworkd-glib)
PKG_CHECK_MODULES(GLIB, glib-2.0)

PKG_CHECK_MODULES(E, [
	evas
	ecore
	ecore-evas
	edje
	etk
	elementary
])

AC_SUBST(LIBFRAMEWORKD_GLIB_CFLAGS)
AC_SUBST(LIBFRAMEWORKD_GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIC_LIBS)
AC_SUBST(E_CFLAGS)
AC_SUBST(E_LIBS)

AC_ARG_WITH(edje-cc,
[  --with-edje-cc=PATH              specify a specific path to edje_cc],
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
])
AC_SUBST(edje_cc)


# output stuff
AC_OUTPUT([
Makefile
data/Makefile
src/Makefile
libframeworkd-phonegui-efl.pc
])

