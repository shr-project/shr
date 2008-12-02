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

static gboolean speaker_active = FALSE;

void call_button_keypad_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info);
void call_button_accept_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info);
void call_button_release_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info);
void call_button_speaker_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info);
void call_button_dtmf_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info);

void call_dtmf_enable(struct CallViewData *data);
void call_dtmf_disable(struct CallViewData *data);
void call_speaker_enable();
void call_speaker_disable();

#endif

