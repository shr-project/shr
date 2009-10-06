#ifndef _CALL_COMMON_H
#define _CALL_COMMON_H

#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-audio.h>
#include "views.h"

enum ActiveCallState {
	CALL_STATE_ACTIVE,
	CALL_STATE_PENDING
};

struct CallViewData {
	struct Window *win;
	GHashTable *options;
	int id;
	char *number;
	gboolean dtmf_active;
	Evas_Object *keypad;
};

struct CallActiveViewData {
	struct CallViewData parent;
	Evas_Object *bt_call_state, *bt_sound_state, *bt_keypad;
	Evas_Object *information, *number;
	enum ActiveCallState state;
};

struct CallIncomingViewData {
	struct CallViewData parent;
	Evas_Object *bt_accept, *bt_reject, *bt_sound_state;	//albacore
	Evas_Object *information, *number;
};

//albacore
enum _CallSoundMode {
	CALL_SOUND_MODE_ACTIVE,
	CALL_SOUND_MODE_SILENT,
	CALL_SOUND_MODE_INIT
};
//end albacore
typedef enum _CallSoundMode CallSoundMode;

enum _CallSoundState {
	CALL_SOUND_STATE_SPEAKER,
	CALL_SOUND_STATE_HANDSET,
	CALL_SOUND_STATE_HEADSET,
	CALL_SOUND_STATE_BT,
	CALL_SOUND_STATE_CLEAR,
	CALL_SOUND_STATE_INIT
};
typedef enum _CallSoundState CallSoundState;

int call_common_active_call_add(struct CallActiveViewData *win);
int call_common_active_call_remove(int id);
int call_common_active_call_get_last_id();

void call_common_window_to_pending(struct CallActiveViewData *win);
void call_common_window_to_active(struct CallActiveViewData *win);
void call_common_window_new_active(int id);
void call_common_window_update_state(struct CallActiveViewData *win,
				     CallSoundState state);

void call_common_window_update_state(struct CallActiveViewData *win,
				     CallSoundState state);
int call_common_set_sound_state(CallSoundState state);
CallSoundState call_common_get_sound_state();
//albacore
GQueue *call_common_get_active_calls_list();
int call_common_set_sound_mode(CallSoundMode mode);
CallSoundMode call_common_get_sound_mode();
void call_common_window_update_mode(struct CallIncomingViewData *win,
				    CallSoundMode mode);
//end albacore


void call_button_keypad_clicked(struct CallViewData *data, Evas_Object * obj,
				void *event_info);
void call_dtmf_enable(struct CallViewData *data);
void call_dtmf_disable(struct CallViewData *data);

#endif
