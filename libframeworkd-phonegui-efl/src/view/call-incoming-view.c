#include "views.h"
#include "call-common.h"

struct CallIncomingViewData {
    struct CallViewData parent;
    Evas_Object *bt1, *bt2;
};


struct CallIncomingViewData *call_incoming_view_show(struct Window *win, GHashTable *options) {
    g_debug("call_incoming_view_show()");

    struct CallIncomingViewData *data = g_slice_alloc0(sizeof(struct CallIncomingViewData));
    data->parent.options = options;
    data->parent.win = win;
    data->parent.id = g_hash_table_lookup(options, "id");
    data->parent.number = g_hash_table_lookup(options, "number");
    data->parent.dtmf_active = FALSE;

    window_layout_set(win, CALL_FILE, "incoming_call");
    window_text_set(win, "number", data->parent.number);

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "Accept");
    evas_object_smart_callback_add(data->bt1, "clicked", call_button_accept_clicked, data);
    window_swallow(win, "button_accept", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, "Release");
    evas_object_smart_callback_add(data->bt2, "clicked", call_button_release_clicked, data);
    window_swallow(win, "button_release", data->bt2);
    evas_object_show(data->bt2);

    return data;
}

void call_incoming_view_hide(struct CallIncomingViewData *data) {
    g_debug("call_incoming_view_hide()");

    struct Window *win = data->parent.win;

    window_unswallow(win, data->bt1);
    evas_object_del(data->bt1);

    window_unswallow(win, data->bt2);
    evas_object_del(data->bt2);

    if(data->parent.dtmf_active) {
        call_dtmf_disable(&(data->parent));
    }

    if(speaker_active) {
        call_speaker_disable();
    }
}

