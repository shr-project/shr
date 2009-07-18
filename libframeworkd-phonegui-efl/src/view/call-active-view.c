#include "views.h"
#include "call-common.h"

static void call_button_speaker_clicked(struct CallActiveViewData *data, Evas_Object *obj, void *event_info);
static void call_button_dtmf_clicked(struct CallActiveViewData *data, Evas_Object *obj, void *event_info);
static void call_button_release_clicked(struct CallActiveViewData *data, Evas_Object *obj, void *event_info);

struct CallActiveViewData *
call_active_view_show(struct Window *win, GHashTable *options)
{
	g_debug("call_active_show()");

	struct CallActiveViewData *data = g_slice_alloc0(sizeof(struct CallActiveViewData));
	data->parent.options = options;
	data->parent.win = win;
	data->parent.id = g_hash_table_lookup(options, "id");
	data->parent.number = g_hash_table_lookup(options, "number");
	data->parent.dtmf_active = FALSE;
	data->state = CALL_STATE_ACTIVE;

	window_layout_set(win, CALL_FILE, "call");

	data->number = elm_label_add( window_evas_object_get(win) );
	elm_label_label_set( data->number,  data->parent.number);
	window_swallow(win, "number", data->number);
	evas_object_show(data->number);

	data->information = elm_label_add( window_evas_object_get(win) );
	elm_label_label_set( data->information,  D_("Active call"));
	window_swallow(win, "text", data->information);
	evas_object_show(data->information);

	data->bt1 = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt1, D_("Release"));
	evas_object_smart_callback_add(data->bt1, "clicked", call_button_release_clicked, data);
	window_swallow(win, "button_release", data->bt1);
	evas_object_show(data->bt1);

	data->bt2 = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt2, D_("Speaker"));
	evas_object_smart_callback_add(data->bt2, "clicked", call_button_speaker_clicked, data);
	window_swallow(win, "button_speaker", data->bt2);
	evas_object_show(data->bt2);

	data->bt3 = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt3, D_("Keypad"));
	evas_object_smart_callback_add(data->bt3, "clicked", call_button_dtmf_clicked, data);
	window_swallow(win, "button_dtmf", data->bt3);
	evas_object_show(data->bt3);

        call_common_active_call_add(data);
	return data;
}

void
call_active_view_hide(struct CallActiveViewData *data)
{
	g_debug("call_active_hide()");

	struct Window *win = data->parent.win;

	if(data->parent.dtmf_active) {
		call_dtmf_disable(&(data->parent));
	}

	if(speaker_active) {
		call_speaker_disable();
	}

	evas_object_del(data->information);
	evas_object_del(data->number);
	evas_object_del(data->bt1);
	evas_object_del(data->bt2);
	evas_object_del(data->bt3);
}

/* FIXME: Should fix to handle bt/headset as well */
static void
call_button_speaker_clicked(struct CallActiveViewData *data, Evas_Object *obj, void *event_info)
{
	g_debug("speaker_clicked()");
	if(speaker_active) {
		speaker_active = FALSE;
		call_speaker_enable();
		//window_text_set(data->win, "text_speaker", D_("Speaker"));
		elm_button_label_set(data->bt2, D_("Speaker"));
	}
	else {
		speaker_active = TRUE;
		call_speaker_disable();
		//window_text_set(data->win, "text_speaker", D_("No Speaker"));
		elm_button_label_set(data->bt2, D_("Handset"));
	}
}

static void
call_button_dtmf_clicked(struct CallActiveViewData *data, Evas_Object *obj, void *event_info)
{
	g_debug("dtmf_clicked()");
	if(data->parent.dtmf_active) {
		data->parent.dtmf_active = FALSE;
		call_dtmf_disable(data);
		//window_text_set(data->win, "text_dtmf", D_("Show Keypad"));
		elm_button_label_set(data->bt3, D_("Keypad"));	
	}
	else {
		data->parent.dtmf_active = TRUE;
		call_dtmf_enable(data);
		//window_text_set(data->win, "text_dtmf", D_("Hide Keypad"));
		 elm_button_label_set(data->bt3, D_("Hide Keypad"));
	}
}

void
call_button_release_clicked(struct CallActiveViewData *data, Evas_Object *obj, void *event_info)
{
	g_debug("release_clicked()");
	if (data->state == CALL_STATE_ACTIVE) {
		ogsmd_call_release(data->parent.id, NULL, NULL);
	}
	else if (data->state == CALL_STATE_PENDING) {
		ogsmd_call_activate(data->parent.id, NULL, NULL);
		call_common_window_new_active(data->parent.id);
		data->state = CALL_STATE_ACTIVE;
	}
	else {
		g_debug("bad state, BUG! shouldn't have gotten here");
	}
}
