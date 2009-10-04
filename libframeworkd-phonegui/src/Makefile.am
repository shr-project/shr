MAINTAINERCLEANFILES = Makefile.in

localedir = $(datadir)/locale
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @GLIB_CFLAGS@ @DBUS_GLIB_CFLAGS@ @FRAMEWORKD_GLIB_CFLAGS@ @LIBPHONE_UTILS_CFLAGS@

lib_LTLIBRARIES = libframeworkd-phonegui.la

libframeworkd_phonegui_la_SOURCES = frameworkd-phonegui.c frameworkd-phonegui.h frameworkd-phonegui-utility.h frameworkd-phonegui-utility.c 
libframeworkd_phonegui_HEADERS = frameworkd-phonegui.h frameworkd-phonegui-utility.h


libframeworkd_phoneguidir = $(includedir)/frameworkd-phonegui

libframeworkd_phonegui_la_LDFLAGS = $(all_libraries) -ldl

libframeworkd_phonegui_la_LIBADD = @GLIB_LIBS@ @DBUS_GLIB_LIBS@ @FRAMEWORKD_GLIB_LIBS@ @LIBPHONE_UTILS_LIBS@
