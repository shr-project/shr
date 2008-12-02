#ifndef _VIEWS_H
#define _VIEWS_H

#include <dbus/dbus-glib.h>
#include <glib.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Etk.h>
#include <stdlib.h>
#include <assert.h>
#include "helper.h"
#include "elm_my_contactlist.h"
#include "async.h"
#include "window.h"

#define CALL_FILE "/usr/share/libframeworkd-phonegui-efl/call.edj"
#define CONTACTS_FILE "/usr/share/libframeworkd-phonegui-efl/contacts.edj"
#define DIALER_FILE "/usr/share/libframeworkd-phonegui-efl/dialer.edj"
#define DIALOG_FILE "/usr/share/libframeworkd-phonegui-efl/dialog.edj"
#define INCOMING_MESSAGE_FILE "/usr/share/libframeworkd-phonegui-efl/incoming-message.edj"
#define KEYPAD_FILE "/usr/share/libframeworkd-phonegui-efl/keypad.edj"
#define MESSAGE_FILE "/usr/share/libframeworkd-phonegui-efl/messages.edj"
#define SIM_AUTH_FILE "/usr/share/libframeworkd-phonegui-efl/sim-auth.edj"

struct DialerViewData *dialer_view_show(struct Window *win, GHashTable *options);
void dialer_view_hide(struct DialerViewData *data);

struct MessageNewViewData *message_new_view_show(struct Window *win, GHashTable *options);
void message_new_view_hide(struct MessageNewViewData *data);

struct MessageFolderViewData *message_folder_view_show(struct Window *win, GHashTable *options);
void message_folder_view_hide(struct MessageFolderViewData *data);

struct MessageShowViewData *message_show_view_show(struct Window *win, GHashTable *options);
void message_show_view_hide(struct MessageShowViewData *data);

struct MessageListViewData *message_list_view_show(struct Window *win, GHashTable *options);
void messages_list_view_hide(struct MessageListViewData *data);

struct SimAuthInputViewData *sim_auth_input_view_show(struct Window *win, GHashTable *options);
void sim_auth_input_view_hide(struct SimAuthInputViewData *data);

struct CallIncomingViewData *call_incoming_view_show(struct Window *win, GHashTable *options);
void call_incoming_view_hide(struct CallIncomingViewData *data);

struct CallActiveViewData *call_active_view_show(struct Window *win, GHashTable *options);
void call_active_view_hide(struct CallActiveViewData *data);

struct ContactListViewData *contact_list_view_show(struct Window *win, GHashTable *options);
void contact_list_view_hide(struct ContactListViewData *data);

struct ContactEditViewData *contact_edit_view_show(struct Window *win, GHashTable *options);
void contact_edit_view_hide(struct ContactEditViewData *data);

#endif

