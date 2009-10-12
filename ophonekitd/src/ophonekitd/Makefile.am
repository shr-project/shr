MAINTAINERCLEANFILES = Makefile.in

ophonekitd-usage-service-glue.h: ${top_builddir}/src/ophonekitd/dbus/ophonekitd-usage.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=ophonekitd_usage_service $^

EXTRA_DIST = dbus/ophonekitd-usage.xml

bin_PROGRAMS = ophonekitd
dbus_sources = ophonekitd-usage-service-glue.h


BUILT_SOURCES = $(dbus_sources)

ophonekitd_SOURCES = $(dbus_sources) \
	ophonekitd-main.c \
	ophonekitd-main.h \
	ophonekitd-fso.c \
	ophonekitd-fso.h \
	ophonekitd-dbus-usage.c \
	ophonekitd-dbus-usage.h \
	ophonekitd-dbus.c \
	ophonekitd-dbus.h \
	ophonekitd-dbus-common.h \
	ophonekitd-phonegui-dbus.c \
	ophonekitd-phonegui-dbus.h

ophonekitd_CFLAGS = \
	-DDATADIR=\"$(datadir)\" \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	@DBUS_GLIB_CFLAGS@ \
	@DBUS_CFLAGS@ \
	@FRAMEWORKD_GLIB_CFLAGS@ \
	@FRAMEWORKD_PHONEGUI_LIBS@  \
	-ggdb

ophonekitd_LDFLAGS = \
	-ldl \
	@DBUS_GLIB_LIBS@ \
	@DBUS_LIBS@ \
	@FRAMEWORKD_GLIB_LIBS@ \
	@FRAMEWORKD_PHONEGUI_LIBS@ 

