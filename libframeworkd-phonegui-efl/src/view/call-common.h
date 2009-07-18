#ifndef _CALL_COMMON_H
#define _CALL_COMMON_H

#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-audio.h>

struct CallViewData {
    struct Window *win;
    GHashTable *options;
    int id;
    char *number;
    gboolean dtmf_active;
    Evas_Object *keypad;
};

enum ActiveCallState {
	CALL_STATE_ACTIVE,
	CALL_STATE_PENDING
};

struct CallActiveViewData {
    struct CallViewData parent;
    Evas_Object *bt1, *bt2, *bt3;
    Evas_Object *information, *number;
    enum ActiveCallState state;
};

typedef struct _CallsList CallsList;
int call_common_active_call_add(int id, struct CallActiveViewData *win);
int call_common_active_call_remove(int id);
struct CallActiveViewData *call_common_active_call_get_window(int id);
void call_common_window_to_pending(struct CallActiveViewData *win);
void call_common_window_to_active(struct CallActiveViewData *win);
void call_common_window_new_active(int id);

static gboolean speaker_active = FALSE;

void call_button_keypad_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info);
void call_dtmf_enable(struct CallViewData *data);
void call_dtmf_disable(struct CallViewData *data);
void call_speaker_enable();
void call_speaker_disable();

#endif

