#include "views.h"
#include "call-common.h"

struct CallActiveViewData {
    struct CallViewData parent;
    Evas_Object *bt1, *bt2, *bt3;
};


struct CallActiveViewData *call_active_view_show(struct Window *win, GHashTable *options) {
    g_debug("call_active_show()");

    struct CallActiveViewData *data = g_slice_alloc0(sizeof(struct CallActiveViewData));
    data->parent.options = options;
    data->parent.win = win;
    data->parent.id = g_hash_table_lookup(options, "id");
    data->parent.number = g_hash_table_lookup(options, "number");
    data->parent.dtmf_active = FALSE;

    window_layout_set(win, CALL_FILE, "call");
    window_text_set(win, "number", data->parent.number);

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "Release");
    evas_object_smart_callback_add(data->bt1, "clicked", call_button_release_clicked, data);
    window_swallow(win, "button_release", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, "Speaker");
    evas_object_smart_callback_add(data->bt2, "clicked", call_button_speaker_clicked, data);
    window_swallow(win, "button_speaker", data->bt2);
    evas_object_show(data->bt2);

    data->bt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt3, "Keypad");
    evas_object_smart_callback_add(data->bt3, "clicked", call_button_dtmf_clicked, data);
    window_swallow(win, "button_dtmf", data->bt3);
    evas_object_show(data->bt3);

    return data;
}

void call_active_view_hide(struct CallActiveViewData *data) {
    g_debug("call_active_hide()");

    struct Window *win = data->parent.win;

    if(data->parent.dtmf_active) {
        call_dtmf_disable(&(data->parent));
    }

    if(speaker_active) {
        call_speaker_disable();
    }

    window_unswallow(win, data->bt1);
    evas_object_del(data->bt1);

    window_unswallow(win, data->bt2);
    evas_object_del(data->bt2);

    window_unswallow(win, data->bt3);
    evas_object_del(data->bt3);
}
