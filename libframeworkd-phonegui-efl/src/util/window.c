#include "window.h"
#include <stdlib.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

static int window_counter = 0;
static void _window_delete_callback(void *data, Evas_Object *win, void *event_info);


struct Window *window_new(char *title) {
    struct Window *win;
    win = g_slice_alloc0(sizeof(struct Window));
    win->title = strdup(title);

    window_counter++;
    return win;
}

void window_init(struct Window *win) {
    g_debug("window_init(win=%d)", win);

    // Window
    win->win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
    assert(win->win != NULL);
    elm_win_title_set(win->win, win->title);
    elm_win_autodel_set(win->win, 1); // Disable it?
    evas_object_smart_callback_add(win->win, "delete-request", _window_delete_callback, NULL);

    // Background
    win->bg = elm_bg_add(win->win);
    evas_object_size_hint_weight_set(win->bg, 1.0, 1.0);
    elm_win_resize_object_add(win->win, win->bg);
    evas_object_show(win->bg);

    // Layout
    win->layout = elm_layout_add(win->win);
    elm_win_resize_object_add(win->win, win->layout);
    evas_object_show(win->layout);

    evas_object_resize(win->win, 480, 600);
}

void window_show(struct Window *win) {
    assert(win != NULL);
    evas_object_show(win->win);
}

void window_layout_set(struct Window *win, const char *file, const char *part) {
    assert(win != NULL);
    elm_layout_file_set(win->layout, file, part);
}

Evas_Object *window_layout_get(struct Window *win) {
    assert(win != NULL);
    return win->layout;
}

Evas_Object *window_evas_object_get(struct Window *win) {
    assert(win != NULL);
    return win->win;
}

void window_delete_callback_set(struct Window *win, void (*cb)(void *data, Evas_Object *win, void *event_info)) {
    assert(win != NULL);
    assert(cb != NULL);
    evas_object_smart_callback_add(win->win, "delete-request", cb, win);
}

void window_text_set(struct Window *win, const char *key, const char *value) {
    assert(win != NULL);
    edje_object_part_text_set(elm_layout_edje_get(win->layout), key, value);
}

void window_swallow(struct Window *win, const char *key, Evas_Object *object) {
    assert(win != NULL);
    edje_object_part_swallow(elm_layout_edje_get(win->layout), key, object);
}

void window_unswallow(struct Window *win, Evas_Object *object) {
    assert(win != NULL);
    edje_object_part_unswallow(elm_layout_edje_get(win->layout), object);
}

void window_view_show(struct Window *win, void *options, void *(*show_cb)(struct Window *win, void *options), void (*hide_cb)(void *data)) {
    g_debug("window_view_show()");
    assert(win != NULL);
    assert(show_cb != NULL);

    // Clear old view
    window_view_hide(win, NULL);

    if(show_cb != NULL) {
        g_debug("Showing view");
        win->view_data = show_cb(win, options);
        g_debug("view_data = %d", win->view_data);
    }

    win->view_hide_cb = hide_cb;
    g_debug("hide_cb = %d", hide_cb);
}

void window_view_hide(struct Window *win, void *options) {
    g_debug("window_view_hide()");
    assert(win != NULL);

    // Hide last frame
    window_frame_hide(win, win->view_data); 

    g_debug("hide_cb = %d", win->view_hide_cb);
    if(win->view_hide_cb != NULL) {
        g_debug("Hiding view");
        g_debug("view_data = %d", win->view_data);
        win->view_hide_cb(win->view_data, options);
        win->view_hide_cb = NULL;
    }

    win->view_data = NULL;
}



void window_frame_show(struct Window *win, void *data, void (*show_cb)(void *data), void (*hide_cb)(void *data)) {
    assert(show_cb != NULL);

    if(win->frame_hide_cb != NULL) {
        g_debug("Hiding frame");
        win->frame_hide_cb(data);
    } else {
        g_debug("No frame to hide");
    }

    // hide_cb could be NULL!
    win->frame_hide_cb = hide_cb;

    g_debug("Showing frame");
    show_cb(data);
}

void window_frame_hide(struct Window *win, void *data) {
    assert(win != NULL);

    if(win->frame_hide_cb != NULL) {
        win->frame_hide_cb(data);
        win->frame_hide_cb = NULL;
    }
}

void window_kbd_show(struct Window *win, KeyboardMode mode) {
    ecore_x_e_virtual_keyboard_state_set(
        ecore_evas_software_x11_window_get(ecore_evas_ecore_evas_get(evas_object_evas_get(win->win))),
        mode
    );
}

void window_kbd_hide(struct Window *win) {
    ecore_x_e_virtual_keyboard_state_set(
        ecore_evas_software_x11_window_get(ecore_evas_ecore_evas_get(evas_object_evas_get(win->win))),
        KEYBOARD_OFF
    );
}

void window_destroy(struct Window *win, void *options) {
    g_debug("window_destroy(win=%d)", win);
    assert(win != NULL);
    window_view_hide(win, options);

    evas_object_hide(win);
    evas_object_del(win->layout);
    evas_object_del(win->bg);
    evas_object_del(win->win);
    g_slice_free1(sizeof(struct Window), win);

    window_counter--;
    if(window_counter == 0) {
        g_debug("DONE THINGS");
        exit(0);
    }
}


static void _window_delete_callback(void *data, Evas_Object *win, void *event_info) {
    window_counter--;
    if(window_counter == 0) {
        g_debug("DONE THINGS");
        exit(0);
    }
}

