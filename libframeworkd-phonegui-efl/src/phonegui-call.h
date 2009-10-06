#ifndef PHONEGUI_CALL_H
#define PHONEGUI_CALL_H

void phonegui_backend_incoming_call_show(const int id, const int status, const char *number);
void phonegui_backend_incoming_call_hide(const int id);
void phonegui_backend_outgoing_call_show(const int id, const int status, const char *number);
void phonegui_backend_outgoing_call_hide(const int id);

#endif
