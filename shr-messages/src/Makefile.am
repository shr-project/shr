MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/libhito

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" -Wall \
	$(FRAMEWORKD_GLIB_CFLAGS) \
	$(FRAMEWORKD_PHONEGUI_CFLAGS)

AM_LDFLAGS = \
	$(FRAMEWORKD_GLIB_LIBS) \
	$(FRAMEWORKD_PHONEGUI_LIBS)

bin_PROGRAMS=shr-messages

shr_messages_SOURCES = \
	messages-main.c \
	messages-main.h 
        

