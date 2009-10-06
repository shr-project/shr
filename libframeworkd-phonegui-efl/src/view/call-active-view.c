#include "views.h"
#include "call-common.h"

static void call_button_sound_state_clicked(struct CallActiveViewData *data,
					    Evas_Object * obj,
					    void *event_info);
static void call_button_dtmf_clicked(struct CallActiveViewData *data,
				     Evas_Object * obj, void *event_info);
static void call_button_state_clicked(struct CallActiveViewData *data,
				      Evas_Object * obj, void *event_info);

struct CallActiveViewData *
call_active_view_show(struct Window *win, GHashTable * options)
{
	g_debug("call_active_show()");

	struct CallActiveViewData *data =
		g_slice_alloc0(sizeof(struct CallActiveViewData));
	data->parent.options = options;
	data->parent.win = win;
	data->parent.id = g_hash_table_lookup(options, "id");
	data->parent.number = g_hash_table_lookup(options, "number");
	data->parent.dtmf_active = FALSE;
	data->state = CALL_STATE_ACTIVE;

	window_layout_set(win, CALL_FILE, "call");

	data->number = elm_label_add(window_evas_object_get(win));
	elm_label_label_set(data->number, data->parent.number);
	window_swallow(win, "number", data->number);
	evas_object_show(data->number);

	data->information = elm_label_add(window_evas_object_get(win));
	elm_label_label_set(data->information, D_("Active call"));
	window_swallow(win, "text", data->information);
	evas_object_show(data->information);

	data->bt_call_state = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_call_state, D_("Release"));
	evas_object_smart_callback_add(data->bt_call_state, "clicked",
				       call_button_state_clicked, data);
	window_swallow(win, "button_release", data->bt_call_state);
	evas_object_show(data->bt_call_state);

	data->bt_sound_state = elm_button_add(window_evas_object_get(win));
	evas_object_smart_callback_add(data->bt_sound_state, "clicked",
				       call_button_sound_state_clicked, data);
	window_swallow(win, "button_speaker", data->bt_sound_state);
	call_common_window_update_state(data, call_common_get_sound_state());
	evas_object_show(data->bt_sound_state);

	data->bt_keypad = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_keypad, D_("Keypad"));
	evas_object_smart_callback_add(data->bt_keypad, "clicked",
				       call_button_dtmf_clicked, data);
	window_swallow(win, "button_dtmf", data->bt_keypad);
	evas_object_show(data->bt_keypad);

	call_common_active_call_add(data);
	return data;
}

void
call_active_view_hide(struct CallActiveViewData *data)
{
	g_debug("call_active_hide()");

	struct Window *win = data->parent.win;

	if (data->parent.dtmf_active) {
		call_dtmf_disable(&(data->parent));
	}

	evas_object_del(data->information);
	evas_object_del(data->number);
	evas_object_del(data->bt_call_state);
	evas_object_del(data->bt_sound_state);
	evas_object_del(data->bt_keypad);
}

/* FIXME: Should fix to handle bt/headset as well */
static void
call_button_sound_state_clicked(struct CallActiveViewData *data,
				Evas_Object * obj, void *event_info)
{
	CallSoundState state = call_common_get_sound_state();
	g_debug("sound_state_clicked(id=%d,state=%d)", data->parent.id, state);

	if (state == CALL_SOUND_STATE_SPEAKER) {
		call_common_set_sound_state(CALL_SOUND_STATE_HANDSET);
	}
	else if (state == CALL_SOUND_STATE_HANDSET) {
		call_common_set_sound_state(CALL_SOUND_STATE_SPEAKER);
	}
	/* else if moree.... */
	else {
		g_debug("trying to set unknown sound state %d failed!", state);
	}
}

static void
call_button_dtmf_clicked(struct CallActiveViewData *data, Evas_Object * obj,
			 void *event_info)
{
	g_debug("dtmf_clicked()");
	if (data->parent.dtmf_active) {
		data->parent.dtmf_active = FALSE;
		call_dtmf_disable(data);
		elm_button_label_set(data->bt_keypad, D_("Keypad"));
	}
	else {
		data->parent.dtmf_active = TRUE;
		call_dtmf_enable(data);
		elm_button_label_set(data->bt_keypad, D_("Hide Keypad"));
	}
}

void
call_button_state_clicked(struct CallActiveViewData *data, Evas_Object * obj,
			  void *event_info)
{
	g_debug("state_clicked(id=%d,state=%d)", data->parent.id, data->state);
	if (data->state == CALL_STATE_ACTIVE) {
		ogsmd_call_release(data->parent.id, NULL, NULL);
	}
	else if (data->state == CALL_STATE_PENDING) {
		ogsmd_call_activate(data->parent.id, NULL, NULL);
		call_common_window_new_active(data->parent.id);
	}
	else {
		g_debug("bad state, BUG! shouldn't have gotten here");
	}
}
