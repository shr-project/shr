MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/util -I$(top_srcdir)/src/widget -I$(top_srcdir)/src/view -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\" @E_CFLAGS@ @LIBFRAMEWORKD_GLIB_CFLAGS@ @GLIB_CFLAGS@ -g3

lib_LTLIBRARIES = libframeworkd-phonegui-efl.la
libframeworkd_phonegui_efl_la_LIBADD = @LIBFRAMEWORKD_GLIB_LIBS@ @GLIB_LIBS@ @E_LIBS@ -lpthread
libframeworkd_phonegui_efl_la_LDFLAGS = -version-info 0:0:0 -no-undefined
libframeworkd_phonegui_efldir = $(includedir)/frameworkd-phonegui-efl
libframeworkd_phonegui_efl_HEADERS = 

libframeworkd_phonegui_efl_la_SOURCES = \
	phonegui-init.h phonegui-init.c \
	phonegui-dialer.h phonegui-dialer.c \
	phonegui-dialog.h phonegui-dialog.c \
	phonegui-messages.h phonegui-messages.c \
	phonegui-incoming-message.h phonegui-incoming-message.c \
	phonegui-sim-auth.h phonegui-sim-auth.c \
	phonegui-call.h phonegui-call.c \
	phonegui-contacts.h phonegui-contacts.c \
	phonegui-ussd.h phonegui-ussd.c \
	widget/etk_tree_model_edje.h widget/etk_tree_model_edje.c \
	widget/elm_keypad.h widget/elm_keypad.c \
	widget/elm_my_contactlist.h widget/elm_my_contactlist.c \
	util/pipe.h util/pipe.c \
	util/helper.h util/helper.c \
	util/instance.h util/instance.c \
	util/async.h util/async.c \
	util/window.h util/window.c \
	util/view.h util/view.c \
	view/dialer-view.h view/dialer-view.c \
	view/dialog-view.h view/dialog-view.c \
	view/contact-edit-view.h view/contact-edit-view.c \
	view/contact-delete-view.h view/contact-delete-view.c \
	view/contact-list-view.h view/contact-list-view.c \
	view/message-new-view.h view/message-new-view.c \
	view/message-delete-view.h view/message-delete-view.c \
	view/message-show-view.h view/message-show-view.c \
	view/message-list-view.h view/message-list-view.c \
	view/message-folder-view.h view/message-folder-view.c \
	view/call-common.h view/call-common.c \
	view/call-incoming-view.h view/call-incoming-view.c \
	view/call-active-view.h view/call-active-view.c \
	view/sim-auth-input-view.h view/sim-auth-input-view.c \
	view/ussd-view.h view/ussd-view.c

