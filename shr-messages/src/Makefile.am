MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/libhito

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" -Wall \
	$(FRAMEWORKD_PHONEGUI_CFLAGS)

AM_LDFLAGS = \
	$(FRAMEWORKD_PHONEGUI_LIBS)

bin_PROGRAMS=shr-messages

shr_messages_SOURCES = messages-main.c
        

