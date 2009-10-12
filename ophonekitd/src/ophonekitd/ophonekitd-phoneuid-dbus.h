#ifndef _OPHONEKITD_PHONEUID_DBUS_H
#define _OPHONEKITD_PHONEUID_DBUS_H

/* --- org.shr.phoneuid.CallManagement --- */
void
phoneuid_call_management_show_incoming(int callid, int status,
		const char *number);
void
phoneuid_call_management_hide_incoming(int callid);

void
phoneuid_call_management_show_outgoing(int callid, int status,
		const char *number);
void
phoneuid_call_management_hide_outgoing(int callid);

/* --- org.shr.phoneuid.Messages --- */
void
phoneuid_messages_display_item(const char *message_path);

/* --- org.shr.phoneuid.Dialogs --- */
void
phoneuid_dialogs_show_dialog(int dialog);
void
phoneuid_dialogs_show_sim_auth(int status);
void
phoneuid_dialogs_hide_sim_auth(int status);
void
phoneuid_dialogs_show_ussd(int mode, const char *message);

#endif
