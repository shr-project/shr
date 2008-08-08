localedir = $(datadir)/locale
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\" $(CONTACTS_CFLAGS) -Wall
AM_LDFLAGS = $(CONTACTS_LIBS)

noinst_LIBRARIES = libhito.a
libhito_a_SOURCES = \
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
	hito-vcard-util.c hito-vcard-util.h

noinst_PROGRAMS = test-widgets
test_widgets_SOURCES = test-widgets.c
test_widgets_LDADD = ./libhito.a
