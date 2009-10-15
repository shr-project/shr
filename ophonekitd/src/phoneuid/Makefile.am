MAINTAINERCLEANFILES = Makefile.in

phoneuid-contacts-service-glue.h: ${top_builddir}/src/phoneuid/dbus/org.shr.phoneuid.Contacts.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=phoneuid_contacts_service $^

phoneuid-messages-service-glue.h: ${top_builddir}/src/phoneuid/dbus/org.shr.phoneuid.Messages.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=phoneuid_messages_service $^

phoneuid-call-management-service-glue.h: ${top_builddir}/src/phoneuid/dbus/org.shr.phoneuid.CallManagement.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=phoneuid_call_management_service $^

phoneuid-dialer-service-glue.h: ${top_builddir}/src/phoneuid/dbus/org.shr.phoneuid.Dialer.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=phoneuid_dialer_service $^

phoneuid-dialogs-service-glue.h: ${top_builddir}/src/phoneuid/dbus/org.shr.phoneuid.Dialogs.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=phoneuid_dialogs_service $^


bin_PROGRAMS = phoneuid
dbus_sources = phoneuid-contacts-service-glue.h \
	       phoneuid-messages-service-glue.h \
	       phoneuid-call-management-service-glue.h \
	       phoneuid-dialer-service-glue.h \
	       phoneuid-dialogs-service-glue.h


BUILT_SOURCES = $(dbus_sources)

phoneuid_SOURCES = $(dbus_sources) \
	phoneuid.c \
	phoneuid-contacts.c \
	phoneuid-contacts.h \
	phoneuid-messages.c \
	phoneuid-messages.h \
	phoneuid-call-management.c \
	phoneuid-call-management.h \
	phoneuid-dialogs.c \
	phoneuid-dialogs.h \
	phoneuid-dialer.c \
	phoneuid-dialer.h

phoneuid_CFLAGS = \
		  -DDATADIR=\"$(datadir)\" \
		  -DPKGDATADIR=\"$(pkgdatadir)\" \
		  @DBUS_GLIB_CFLAGS@ \
		  @DBUS_CFLAGS@ \
		  @FRAMEWORKD_GLIB_CFLAGS@ \
		  @FRAMEWORKD_PHONEGUI_CFLAGS@ \
		  -ggdb

phoneuid_LDFLAGS = \
		   -ldl \
		   @DBUS_GLIB_LIBS@ \
		   @DBUS_LIBS@ \
		   @FRAMEWORKD_GLIB_LIBS@ \
		   @FRAMEWORKD_PHONEGUI_LIBS@


