MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = ophonekitd

ophonekitd_SOURCES = \
	ophonekitd-main.c \
	ophonekitd-main.h \
	ophonekitd-phonegui.c \
	ophonekitd-phonegui.h

ophonekitd_CFLAGS = \
	-DDATADIR=\"$(datadir)\" \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	@DBUS_GLIB_CFLAGS@ \
	@FRAMEWORKD_GLIB_CFLAGS@ \
	@FRAMEWORKD_PHONEGUI_LIBS@

ophonekitd_LDFLAGS = \
	-ldl \
	@DBUS_GLIB_LIBS@ \
	@FRAMEWORKD_GLIB_LIBS@ \
	@FRAMEWORKD_PHONEGUI_LIBS@
