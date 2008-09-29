AC_PREREQ(2.53)
AC_INIT(openmoko-dialer, 3.0, http://www.openmoko.org/)
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
                  libframeworkd-glib
                  )
                                   
#old_cflags=$CFLAGS
#CFLAGS=$DIALER_CFLAGS
#AC_CHECK_TYPE(EContactPhotoType, [], [], [#include <libebook/e-book.h>])
#CFLAGS=$old_cflags

#if test $ac_cv_type_EContactPhotoType = yes; then
#	AC_DEFINE(HAVE_ECONTACT_PHOTO_TYPE, 1, [Defined if EContactPhotoType exists])
#fi

AC_SUBST(DIALER_CFLAGS)
AC_SUBST(DIALER_LIBS)

# output stuff
AC_CONFIG_FILES([
Makefile
data/Makefile
src/Makefile
])

AC_OUTPUT
