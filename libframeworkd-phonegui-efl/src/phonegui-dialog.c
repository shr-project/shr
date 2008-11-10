#include "phonegui-dialog.h"
#include <glib-2.0/glib.h>
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/dialog.edj"

enum DialogEvents {
    EVENT_SHOW,
    EVENT_HIDE
};

static Evas_Object *bt1;


void phonegui_dialog_show(int type) {
    pipe_write(pipe_handler, dialog_event, EVENT_SHOW);
}

void phonegui_dialog_hide() {
    pipe_write(pipe_handler, dialog_event, EVENT_HIDE);
}


void dialog_delete(Ecore_Evas *ee) {
    g_debug("dialog_delete()");
    phonegui_dialog_hide();
}

void dialog_event(int event) {
    g_debug("dialog_event()");

    if(event == EVENT_SHOW) {
        window_create("Information", dialog_event, dialog_delete);
        frame_show(dialog_main_show, dialog_main_hide);
        evas_object_show(win);
    } else if(event == EVENT_HIDE) {
        evas_object_hide(win);
        window_destroy();
    }
}


/*
 * Button triggers
 */
void dialog_button_close_clicked() {
    phonegui_dialog_hide();
}


/*
 * Views
 */
void dialog_main_show() {
    elm_layout_file_set(layout, UI_FILE, "dialog");
    edje_object_part_text_set(elm_layout_edje_get(layout), "content", "Your storage is full. Please delete some messages or you are not going to receive messages anymore!");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Close");
    evas_object_smart_callback_add(bt1, "clicked", dialog_button_close_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_close", bt1);
    evas_object_show(bt1);
}

void dialog_main_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);
}

