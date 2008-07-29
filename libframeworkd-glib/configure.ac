AC_PREREQ(2.53)
AC_INIT(openmoko-dialer, 0.1, http://www.openmoko.org/)
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
                  dbus-glib-1
                  gtk+-2.0 
                  libebook-1.2 
                  libmokojournal2
                  libmokoui2
                  libpulse)

PKG_CHECK_MODULES(JANA, libjana libjana-ecal libjana-gtk)
PKG_CHECK_MODULES(NOTIFY, libnotify)
PKG_CHECK_MODULES(ALSA, alsa)

old_cflags=$CFLAGS
CFLAGS=$DIALER_CFLAGS
AC_CHECK_TYPE(EContactPhotoType, [], [], [#include <libebook/e-book.h>])
CFLAGS=$old_cflags

if test $ac_cv_type_EContactPhotoType = yes; then
	AC_DEFINE(HAVE_ECONTACT_PHOTO_TYPE, 1, [Defined if EContactPhotoType exists])
fi

AC_SUBST(DIALER_CFLAGS)
AC_SUBST(DIALER_LIBS)

# deps for libhito
PKG_CHECK_MODULES(CONTACTS, glib-2.0 glib-2.0 gtk+-2.0 libebook-1.2)

old_cflags=$CFLAGS
CFLAGS=$CONTACTS_CFLAGS
AC_CHECK_TYPE(EContactPhotoType, [], [], [#include <libebook/e-book.h>])
CFLAGS=$old_cflags

if test $ac_cv_type_EContactPhotoType = yes; then
	AC_DEFINE(HAVE_PHOTO_TYPE, 1, [Defined if EContactPhotoType exists])
fi

# output stuff
AC_OUTPUT([
Makefile
data/Makefile
libhito/Makefile
src/Makefile
src/common/Makefile
src/dialer/Makefile
src/libphonegui/Makefile
src/ophonekitd/Makefile
])
