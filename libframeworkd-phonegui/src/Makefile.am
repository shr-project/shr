localedir = $(datadir)/locale
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @PHONEGUI_CFLAGS@

lib_LTLIBRARIES = libframeworkd-phonegui.la

libframeworkd_phonegui_la_SOURCES = frameworkd-phonegui.c frameworkd-phonegui.h
libframeworkd_phonegui_HEADERS = frameworkd-phonegui.h


libframeworkd_phoneguidir = $(includedir)/frameworkd-phonegui

libframeworkd_phonegui_la_LDFLAGS = $(all_libraries) -ldl

libframeworkd_phonegui_la_LIBADD = @PHONEGUI_LIBS@ 
