AC_PREREQ(2.53)
AC_INIT(libhito, 0.6, http://www.openedhand.com/)
AM_INIT_AUTOMAKE()
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL
AC_GNU_SOURCE
IT_PROG_INTLTOOL([0.35.0])

dnl i18n support
GETTEXT_PACKAGE=libhito
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT

PKG_CHECK_MODULES(LIBEBOOK, glib-2.0 gtk+-2.0 libebook-1.2)

AC_SUBST(LIBEBOOK_CFLAGS)
AC_SUBST(LIBEBOOK_LIBS)

old_cflags=$CFLAGS
CFLAGS=$LIBEBOOK_CFLAGS
AC_CHECK_TYPE(EContactPhotoType, [], [], [#include <libebook/e-book.h>])
CFLAGS=$old_cflags

if test $ac_cv_type_EContactPhotoType = yes; then
	AC_DEFINE(HAVE_PHOTO_TYPE, 1, [Defined if EContactPhotoType exists])
fi

AC_CONFIG_FILES([
Makefile
po/Makefile.in
src/Makefile
libhito.pc
])

AC_OUTPUT
