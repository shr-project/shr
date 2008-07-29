AC_PREREQ(2.53)
AC_INIT(ophonekitd, 0.1, http://www.openmoko.org/)
AM_INIT_AUTOMAKE()
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_GNU_SOURCE
AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

# base deps TODO
PKG_CHECK_MODULES(FRAMEWORKD_GLIB,
                  libframeworkd-glib,
		  [],
		  [AC_MSG_FAILURE([libframeworkd-glib not found])]
		 )
PKG_CHECK_MODULES(FRAMEWORKD_GLIB_PHONEGUI,
                  libframeworkd-glib-phonegui-gtk,
                  [],
                  [AC_MSG_FAILURE([libframeworkd-glib-phonegui-gtk not found])]
                 )
PKG_CHECK_MODULES(DBUS_GLIB,
                  dbus-glib-1,
                  [],
                  [AC_MSG_FAILURE([dbus-glib not found])]
                 )

AC_SUBST(DBUS_GLIB_CFLAGS)
AC_SUBST(DBUS_GLIB_LIBS)
AC_SUBST(FRAMEWORKD_GLIB_CFLAGS)
AC_SUBST(FRAMEWORKD_GLIB_LIBS)
AC_SUBST(FRAMEWORKD_GLIB_PHONEGUI_CFLAGS)
AC_SUBST(FRAMEWORKD_GLIB_PHONEGUI_LIBS)

# output stuff
AC_OUTPUT([
Makefile
src/Makefile
])
