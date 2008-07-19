localedir = $(datadir)/locale
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

noinst_LIBRARIES = libphonegui.a
libphonegui_a_SOURCES = \
	moko-pin.h moko-pin.c
