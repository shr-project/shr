MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/libhito

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
                $(FRAMEWORKD_GLIB_CFLAGS) \
                -Wall

AM_LDFLAGS = @FRAMEWORKD_GLIB_LIBS@ @FRAMEWORKD_PHONEGUI_LIBS@ -ldl

bin_PROGRAMS=shr-contacts

shr_contacts_SOURCES = \
	contacts-main.c \
	contacts-main.h \ 
	contacts-phonegui.c \
	contacts-phonegui.h

        

