#ifndef _OPHONEKITD_PHONEGUI_DBUS_H
#define _OPHONEKITD_PHONEGUI_DBUS_H

/* --- org.shr.ophonekitd.CallManagement --- */
void
phonegui_call_management_show_incoming(int callid, int status,
		const char *number);
void
phonegui_call_management_hide_incoming(int callid);

void
phonegui_call_management_show_outgoing(int callid, int status,
		const char *number);
void
phonegui_call_management_hide_outgoing(int callid);

/* --- org.shr.ophonekitd.Messages --- */
void
phonegui_messages_display_item(const char *message_path);

/* --- org.shr.ophonekitd.Dialogs --- */
void
phonegui_dialogs_show_dialog(int dialog);
void
phonegui_dialogs_show_sim_auth(int status);
void
phonegui_dialogs_hide_sim_auth(int status);
void
phonegui_dialogs_show_ussd(int mode, const char *message);

#endif
