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

# dnl glib-genmarshal
# AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

# base deps
#PKG_CHECK_MODULES(DIALER,
#                  libebook-1.2 
#                  libmokojournal2
#                  libmokoui2
#                  libpulse
#                  libframeworkd-glib)

PKG_CHECK_MODULES(PHONEGUI, libframeworkd-glib)
PKG_CHECK_MODULES(GLIB, glib-2.0)
PKG_CHECK_MODULES(EVAS, evas)
# PKG_CHECK_MODULES(ECORE_EVAS, ecore-evas)
PKG_CHECK_MODULES(EDJE, edje)
PKG_CHECK_MODULES(ECORE, ecore)
PKG_CHECK_MODULES(ETK, etk)


# PKG_CHECK_MODULES(MOKO, libmokojournal2 libmokoui2)
# PKG_CHECK_MODULES(EBOOK, libebook-1.2)
# PKG_CHECK_MODULES(LFG, libframeworkd-glib)
# PKG_CHECK_MODULES(DBUS, dbus-glib-1)
# PKG_CHECK_MODULES(NOTIFY, libnotify)

# old_cflags=$CFLAGS
# CFLAGS=$DIALER_CFLAGS
# AC_CHECK_TYPE(EContactPhotoType, [], [], [#include <libebook/e-book.h>])
# CFLAGS=$old_cflags

# if test $ac_cv_type_EContactPhotoType = yes; then
#	AC_DEFINE(HAVE_ECONTACT_PHOTO_TYPE, 1, [Defined if EContactPhotoType exists])
# fi

#AC_SUBST(PHONEGUI_CFLAGS)
#AC_SUBST(PHONEGUI_LIBS)

# deps for libhito
# PKG_CHECK_MODULES(CONTACTS, glib-2.0 glib-2.0 libebook-1.2)

# old_cflags=$CFLAGS
# CFLAGS=$CONTACTS_CFLAGS
# AC_CHECK_TYPE(EContactPhotoType, [], [], [#include <libebook/e-book.h>])
# CFLAGS=$old_cflags

# if test $ac_cv_type_EContactPhotoType = yes; then
#	AC_DEFINE(HAVE_PHOTO_TYPE, 1, [Defined if EContactPhotoType exists])
# fi

# output stuff
AC_CONFIG_FILES([
Makefile
data/Makefile
src/Makefile
libframeworkd-phonegui-efl.pc
])

AC_OUTPUT
