INCLUDES = -I$(top_srcdir)/libhito

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
                $(MOKOUI_CFLAGS) \
                $(FRAMEWORKD_GLIB_CFLAGS) \
                -Wall

AM_LDFLAGS = $(MOKOUI_LIBS) \
             $(EBOOK_LIBS) \
             $(FRAMEWORKD_GLIB_LIBS)

bin_PROGRAMS=openmoko-messages

openmoko_messages_SOURCES = \
	messages-main.c \
	messages-main.h \ 
	messages-phonegui.c \
	messages-phonegui.h

        

