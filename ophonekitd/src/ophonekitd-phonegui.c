
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gthread.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>
#include "ophonekitd-phonegui.h"
#include "ophonekitd-globals.h"


void *
ophonekitd_phonegui_main(void *_data)
{
	g_debug("[phonegui] starting phonegui thread");
	g_async_queue_ref(phonegui_queue);
	phonegui_load("ophonekitd");
	phonegui_init(ophonekitd_argc, ophonekitd_argv, ophonekitd_phonegui_idle);
	phonegui_loop();
	g_debug("[phonegui] exited from phonegui_loop!");
}

void
ophonekitd_phonegui_command(int command, void *data)
{
	struct PGMessage *msg = malloc(sizeof(struct PGMessage));
	if (!msg) {
		g_warning("[phonegui] Out of memory!!!");
		return;
	}

	msg->command = command;
	msg->data = data;

	g_async_queue_push(phonegui_queue, msg);
}

void
ophonekitd_phonegui_call_command(int command, int id,
		int status, char *number)
{
	struct PGCallPack *data = malloc(sizeof(struct PGCallPack));
	data->id = id;
	data->status = status;
	data->number = number;
	ophonekitd_phonegui_command(command, data);
}

void
ophonekitd_phonegui_ussd_command(int mode, char *message)
{
	struct PGUssdPack *data = malloc(sizeof(struct PGUssdPack));
	data->mode = mode;
	data->message = message;
	ophonekitd_phonegui_command(USSD_SHOW, data);
}


int
ophonekitd_phonegui_idle(void *dummy)
{
	struct PGCallPack *call_pack = NULL;
	struct PGUssdPack *ussd_pack = NULL;
	struct PGMessage *msg =
		(struct PGMessage *)g_async_queue_try_pop(phonegui_queue);
	if (msg) {
		g_debug("[phonegui] idle - got a message :-)");
		switch (msg->command) {
		case INCOMING_CALL_SHOW:
			call_pack = (struct PGCallPack *)msg->data;
			phonegui_incoming_call_show(call_pack->id,
					call_pack->status, call_pack->number);
			break;
		case INCOMING_CALL_HIDE:
			phonegui_incoming_call_hide(GPOINTER_TO_INT(msg->data));
			break;
		case OUTGOING_CALL_SHOW:
			call_pack = (struct PGCallPack *)msg->data;
			phonegui_outgoing_call_show(call_pack->id,
					call_pack->status, call_pack->number);
			break;
		case OUTGOING_CALL_HIDE:
			phonegui_outgoing_call_hide(GPOINTER_TO_INT(msg->data));
			break;
		case SIM_AUTH_SHOW:
			phonegui_sim_auth_show(GPOINTER_TO_INT(msg->data));
			break;
		case SIM_AUTH_HIDE:
			phonegui_sim_auth_hide(GPOINTER_TO_INT(msg->data));
			break;
		case MESSAGE_SHOW:
			phonegui_message_show(GPOINTER_TO_INT(msg->data));
			break;
		case USSD_SHOW:
			ussd_pack = (struct PGUssdPack *)msg->data;
			phonegui_ussd_show(ussd_pack->mode, ussd_pack->message);
			break;
		case DIALOG_SHOW:
			phonegui_dialog_show(GPOINTER_TO_INT(msg->data));
			break;
		default:
			g_debug("[phonegui] unhandled phonegui command!");
			break;
		}
		free(msg);
	}
	return (TRUE);
}
