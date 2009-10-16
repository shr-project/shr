#ifndef _OPHONEKITD_FRAMEWORKD_H
#define _OPHONEKITD_FRAMEWORKD_H



/* GSM resource handling */
gboolean
ophonekitd_list_resources();
gboolean
ophonekitd_request_gsm();
gboolean
ophonekitd_set_antenna_power();
gboolean
ophonekitd_register_network();
void
ophonekitd_sim_ready_actions(void);

/* signal handlers */
void
ophonekitd_resource_available_handler(const char *name,
		gboolean availability);
void
ophonekitd_resource_changed_handler(const char *name, gboolean state,
				    GHashTable * attributes);
void
ophonekitd_device_idle_notifier_power_state_handler(GError * error,
						    const int status,
						    gpointer userdata);
void
ophonekitd_device_idle_notifier_state_handler(const int state);
void
ophonekitd_call_status_handler(const int call_id, const int status,
			       GHashTable * properties);
void
ophonekitd_sim_auth_status_handler(const int status);
void
ophonekitd_sim_ready_status_handler(gboolean status);
void
ophonekitd_incoming_message_handler(char *message_path);
void
ophonekitd_sim_incoming_stored_message_handler(const int id);
void
ophonekitd_incoming_ussd_handler(int mode, const char *message);
void
ophonekitd_network_status_handler(GHashTable *status);

#endif

