AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = $(LIBEBOOK_CFLAGS) -Wall
AM_LDFLAGS = $(LIBEBOOK_LIBS)	

noinst_PROGRAMS = test-widgets
test_widgets_SOURCES = test-widgets.c
test_widgets_LDADD = ./libhito.la

lib_LTLIBRARIES = libhito.la

libhito_la_SOURCES = \
	hito-contact-store.h hito-contact-store.c \
	hito-contact-model-filter.h hito-contact-model-filter.c \
	hito-contact-view.h hito-contact-view.c \
	hito-group.h hito-group.c \
	hito-all-group.h hito-all-group.c \
	hito-category-group.h hito-category-group.c \
	hito-no-category-group.h hito-no-category-group.c \
	hito-separator-group.h hito-separator-group.c \
	hito-group-store.h hito-group-store.c \
	hito-group-combo.h hito-group-combo.c \
	hito-contact-preview.h hito-contact-preview.c \
	hito-vcard-util.h hito-vcard-util.c

libhito_HEADERS = hito-contact-store.h \
  				  hito-contact-model-filter.h \
  				  hito-contact-view.h \
				  hito-group.h \
				  hito-all-group.h \
				  hito-category-group.h \
				  hito-no-category-group.h \
				  hito-separator-group.h \
				  hito-group-store.h \
				  hito-group-combo.h \
				  hito-contact-preview.h \
				  hito-vcard-util.h

libhitodir = $(includedir)/libhito

libhito_la_LDFLAGS = $(all_libraries) -version-info 0:0:0 -no-undefined
