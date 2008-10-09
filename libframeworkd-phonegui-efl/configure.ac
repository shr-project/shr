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
PKG_CHECK_MODULES(ECORE_EVAS, ecore-evas)
PKG_CHECK_MODULES(EDJE, edje)
PKG_CHECK_MODULES(ECORE, ecore)
PKG_CHECK_MODULES(ETK, etk)



# output stuff
AC_CONFIG_FILES([
Makefile
data/Makefile
src/Makefile
libframeworkd-phonegui-efl.pc
])

AC_OUTPUT
