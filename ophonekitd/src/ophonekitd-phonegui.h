#ifndef _OPHONEKITD_PHONEGUI_H
#define _OPHONEKITD_PHONEGUI_H


enum {
	INCOMING_CALL_SHOW,
	INCOMING_CALL_HIDE,
	OUTGOING_CALL_SHOW,
	OUTGOING_CALL_HIDE,
	SIM_AUTH_SHOW,
	SIM_AUTH_HIDE,
	MESSAGE_SHOW,
	USSD_SHOW,
	DIALOG_SHOW
};


struct PGMessage {
	int command;
	void *data;
};

struct PGCallPack {
	int id;
	int status;
	char *number;
};

struct PGUssdPack {
	int mode;
	char *message;
};



void *
ophonekitd_phonegui_main(void *_data);
int
ophonekitd_phonegui_idle(void *);
void
ophonekitd_phonegui_command(int command, void *data);
void
ophonekitd_phonegui_call_command(int command, int id,
		int status, char *number);
void
ophonekitd_phonegui_ussd_command(int mode, char *message);

#endif
