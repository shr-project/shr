#include "views.h"
#include "call-common.h"


void call_button_keypad_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info) {
    char string[2];
    string[0] = (char) event_info;
    string[1] = '\0';
    g_debug("call_button_keypad_clicked(): %s", string);
    ogsmd_call_send_dtmf(strdup(string), NULL, NULL);
}

void call_button_accept_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("accept_clicked()");
    ogsmd_call_activate(data->id, NULL, NULL);

    GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(options, "id", GINT_TO_POINTER(data->id));
    g_hash_table_insert(options, "number", strdup(data->number));
    window_view_show(data->win, options, call_active_view_show, call_active_view_hide);
}

void call_button_release_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("release_clicked()");
    ogsmd_call_release(data->id, NULL, NULL);
}

void call_button_speaker_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("speaker_clicked()");
    if(speaker_active) {
        speaker_active = FALSE;
        call_speaker_enable();
        window_text_set(data->win, "text_speaker", "Speaker");
    } else {
        speaker_active = TRUE;
        call_speaker_disable();
        window_text_set(data->win, "text_speaker", "No Speaker");
    }
}

void call_button_dtmf_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("dtmf_clicked()");
    if(data->dtmf_active) {
        data->dtmf_active = FALSE;
        call_dtmf_disable(data);
        window_text_set(data->win, "text_dtmf", "Show Keypad");
    } else {
        data->dtmf_active = TRUE;
        call_dtmf_enable(data);
        window_text_set(data->win, "text_dtmf", "Hide Keypad");
    }
}





void call_dtmf_enable(struct CallViewData *data) {
    g_debug("call_dtmf_enable()");
    data->keypad = elm_keypad_add(window_evas_object_get(data->win));
    evas_object_smart_callback_add(data->keypad, "clicked", call_button_keypad_clicked, data);
    window_swallow(data->win, "keypad", data->keypad);
    evas_object_show(data->keypad);
}

void call_dtmf_disable(struct CallViewData *data) {
    g_debug("call_dtmf_disable()");
    window_unswallow(data->win, data->keypad);
    evas_object_smart_callback_del(data->keypad, "clicked", call_button_keypad_clicked);
    evas_object_del(data->keypad);
}

void call_speaker_enable() {
    g_debug("call_speaker_enable()");
    odeviced_audio_pull_scenario(NULL, NULL);
}

void call_speaker_disable() {
    g_debug("call_speaker_disable()");
    odeviced_audio_push_scenario("gsmspeakerout", NULL, NULL);
}

