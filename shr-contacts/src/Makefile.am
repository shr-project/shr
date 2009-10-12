MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/libhito

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
                @DBUS_CFLAGS@ \
                -Wall

AM_LDFLAGS = @DBUS_LIBS@

bin_PROGRAMS=shr-contacts

shr_contacts_SOURCES = \
	contacts-main.c

        

