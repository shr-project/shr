localedir = $(datadir)/locale
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

INCLUDES = -I$(top_srcdir)/src/common 

lib_LTLIBRARIES = libframeworkd-glib-phonegui-gtk.la

libframeworkd_glib_phonegui_gtk_la_SOURCES = \
	moko-pin.h moko-pin.c moko-main.c

libframeworkd_glib_phonegui_gtk_la_LDFLAGS = $(all_libraries) -version-info 0:0:0 -no-undefined

libframeworkd_glib_phonegui_gtk_la_LIBADD = @DIALER_LIBS@
