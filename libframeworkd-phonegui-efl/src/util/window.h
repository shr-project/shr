#ifndef _WINDOW_H
#define _WINDOW_H

#include <Ecore_X.h>
#include <Elementary.h>
#include <glib.h>


// TODO: Make the Window structure private!
struct Window {
	Evas_Object *win, *bg, *layout;
	char *title;

	void *view_data;
	void (*view_hide_cb) (void *data, void *options);

	void (*frame_hide_cb) ();
};

typedef enum {
	KEYBOARD_OFF = ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF,
	KEYBOARD_PIN = ECORE_X_VIRTUAL_KEYBOARD_STATE_PIN,
	KEYBOARD_ALPHA = ECORE_X_VIRTUAL_KEYBOARD_STATE_ALPHA,
	KEYBOARD_NUMERIC = ECORE_X_VIRTUAL_KEYBOARD_STATE_NUMERIC
} KeyboardMode;

struct Window *window_new(char *title);
void window_init(struct Window *win);
void window_show(struct Window *win);
void window_layout_set(struct Window *win, const char *file, const char *part);
Evas_Object *window_evas_object_get(struct Window *win);
Evas_Object *window_layout_get(struct Window *win);
void window_delete_callback_set(struct Window *win,
				void (*cb) (void *data, Evas_Object * win,
					    void *event_info));
void window_text_set(struct Window *win, const char *key, const char *value);
void window_swallow(struct Window *win, const char *key, Evas_Object * object);
void window_unswallow(struct Window *win, Evas_Object * object);
void window_view_show(struct Window *win, void *options,
		      void *(*show_cb) (struct Window * win, void *options),
		      void (*hide_cb) (void *data));
void window_view_hide(struct Window *win, void *options);
void window_frame_show(struct Window *win, void *data,
		       void (*show_cb) (void *data),
		       void (*hide_cb) (void *data));
void window_frame_hide(struct Window *win, void *data);
void window_kbd_show(struct Window *win, KeyboardMode mode);
void window_kbd_hide(struct Window *win);
void window_destroy(struct Window *win, void *data);

#endif
