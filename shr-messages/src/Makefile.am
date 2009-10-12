MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" -Wall \
	@DBUS_CFLAGS@

AM_LDFLAGS = @DBUS_LIBS@

bin_PROGRAMS=shr-messages

shr_messages_SOURCES = messages-main.c
        

