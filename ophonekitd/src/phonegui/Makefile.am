MAINTAINERCLEANFILES = Makefile.in

ophonekitd-contacts-service-glue.h: ${top_builddir}/src/phonegui/dbus/org.shr.ophonekitd.Contacts.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=ophonekitd_contacts_service $^

ophonekitd-messages-service-glue.h: ${top_builddir}/src/phonegui/dbus/org.shr.ophonekitd.Messages.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=ophonekitd_messages_service $^

ophonekitd-call-management-service-glue.h: ${top_builddir}/src/phonegui/dbus/org.shr.ophonekitd.CallManagement.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=ophonekitd_call_management_service $^

#EXTRA_DIST = dbus/ophonekitd-usage.xml

bin_PROGRAMS = ophonekitd-phonegui
dbus_sources = ophonekitd-contacts-service-glue.h \
	       ophonekitd-call-management-service-glue.h


BUILT_SOURCES = $(dbus_sources)

ophonekitd_phonegui_SOURCES = $(dbus_sources) \
	ophonekitd-phonegui.c \
	ophonekitd-phonegui.h \
	ophonekitd-phonegui-contacts.c \
	ophonekitd-phonegui-contacts.h \
	ophonekitd-phonegui-messages.c \
	ophonekitd-phonegui-messages.h \
	ophonekitd-phonegui-call-management.c \
	ophonektid-phonegui-call-management.h

ophonekitd_phonegui_CFLAGS = \
		  -DDATADIR=\"$(datadir)\" \
		  -DPKGDATADIR=\"$(pkgdatadir)\" \
		  @DBUS_GLIB_CFLAGS@ \
		  @DBUS_CFLAGS@ \
		  @FRAMEWORKD_GLIB_CFLAGS@ \
		  @FRAMEWORKD_PHONEGUI_CFLAGS@ \
		  -ggdb

ophonekitd_phonegui_LDFLAGS = \
		   -ldl \
		   @DBUS_GLIB_LIBS@ \
		   @DBUS_LIBS@ \
		   @FRAMEWORKD_GLIB_LIBS@ \
		   @FRAMEWORKD_PHONEGUI_LIBS@


