#include "phonegui-dialog.h"
#include <glib-2.0/glib.h>
#include "phonegui-init.h"
#include "phonegui-window.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/dialog.edj"

enum DialogEvents {
    EVENT_SHOW,
    EVENT_HIDE
};


struct DialogWindow {
    struct Window win;
    Evas_Object *bt1;
};

void dialog_delete(Ecore_Evas *ee);
void dialog_event(int event, struct DialogWindow *win);

void dialog_button_close_clicked(struct DialogWindow *win, Evas_Object *obj, void *event_info);

void dialog_main_show(struct DialogWindow *win);
void dialog_main_hide(struct DialogWindow *win);



void phonegui_dialog_show(int type) {
    struct DialogWindow *win = window_new(sizeof(struct DialogWindow), "Information", dialog_delete);
    pipe_write(pipe_handler, dialog_event, EVENT_SHOW, win);
}

void phonegui_dialog_hide() {
    pipe_write(pipe_handler, dialog_event, EVENT_HIDE, NULL);
}


void dialog_delete(Ecore_Evas *ee) {
    g_debug("dialog_delete()");
    phonegui_dialog_hide();
}

void dialog_event(int event, struct DialogWindow *win) {
    g_debug("dialog_event()");

    if(event == EVENT_SHOW) {
        window_init(win);
        window_frame_show(win, dialog_main_show, dialog_main_hide);
        window_show(win);
    } else if(event == EVENT_HIDE) {
        window_destroy(win);
    }
}


/*
 * Button triggers
 */
void dialog_button_close_clicked(struct DialogWindow *win, Evas_Object *obj, void *event_info) {
    phonegui_dialog_hide(NULL);
}


/*
 * Views
 */
void dialog_main_show(struct DialogWindow *win) {
    window_layout_set(win, UI_FILE, "dialog");
    window_text_set(win, "content", "Your storage is full. Please delete some messages or you are not going to receive messages anymore!");

    win->bt1 = elm_button_add(win);
    elm_button_label_set(win->bt1, "Close");
    evas_object_smart_callback_add(win->bt1, "clicked", dialog_button_close_clicked, win);
    window_swallow(win, "button_close", win->bt1);
    evas_object_show(win->bt1);
}

void dialog_main_hide(struct DialogWindow *win) {
    window_unswallow(win, win->bt1);
    evas_object_del(win->bt1);
}

