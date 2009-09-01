#include "views.h"
#include "call-common.h"



static void call_button_accept_clicked(struct CallIncomingViewData *data, Evas_Object *obj, void *event_info);
static void call_button_release_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info);
static void call_button_sound_state_clicked(struct CallIncomingViewData *data, Evas_Object *obj, void *event_info); //albacore

struct CallIncomingViewData*
call_incoming_view_show(struct Window *win, GHashTable *options) {
	g_debug("call_incoming_view_show()");

	struct CallIncomingViewData *data = g_slice_alloc0(sizeof(struct CallIncomingViewData));
	data->parent.options = options;
	data->parent.win = win;
	data->parent.id = GPOINTER_TO_INT(g_hash_table_lookup(options, "id"));
	data->parent.number = g_hash_table_lookup(options, "number");
	data->parent.dtmf_active = FALSE;

	window_layout_set(win, CALL_FILE, "incoming_call");

	data->number = elm_label_add( window_evas_object_get(win) );
	elm_label_label_set( data->number,  data->parent.number);
	window_swallow(win, "number", data->number);
	evas_object_show(data->number);

	data->information = elm_label_add( window_evas_object_get(win) );
	elm_label_label_set( data->information,  D_("Incoming call"));
	window_swallow(win, "information", data->information);
	evas_object_show(data->information);

	data->bt_accept = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_accept, D_("Accept"));
	evas_object_smart_callback_add(data->bt_accept, "clicked", call_button_accept_clicked, data);
	window_swallow(win, "button_accept", data->bt_accept);
	evas_object_show(data->bt_accept);

	data->bt_reject = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_reject, D_("Reject"));
	evas_object_smart_callback_add(data->bt_reject, "clicked", call_button_release_clicked, data);
	window_swallow(win, "button_release", data->bt_reject);
	evas_object_show(data->bt_reject);

	//albacore
	data->bt_sound_state = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_sound_state, D_("Silent"));
	evas_object_smart_callback_add(data->bt_sound_state, "clicked", call_button_sound_state_clicked, data);
	window_swallow(win, "button_silent", data->bt_sound_state);
	call_common_window_update_mode(data, call_common_get_sound_mode());
	evas_object_show(data->bt_sound_state);	
	//end albacore	

	return data;
}

void
call_incoming_view_hide(struct CallIncomingViewData *data) {
	g_debug("call_incoming_view_hide()");

	struct Window *win = data->parent.win;

	evas_object_del(data->information);
	evas_object_del(data->number);
	evas_object_del(data->bt_accept);
	evas_object_del(data->bt_reject);
	evas_object_del(data->bt_sound_state); //albacore

	if(data->parent.dtmf_active) {
		call_dtmf_disable(&data->parent);
	}
}


static void
call_button_accept_clicked(struct CallIncomingViewData *data, Evas_Object *obj, void *event_info)
{
	g_debug("accept_clicked()");
	ogsmd_call_activate(data->parent.id, NULL, NULL);

	GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(options, "id", GINT_TO_POINTER(data->parent.id));
	g_hash_table_insert(options, "number", strdup(data->parent.number));
	window_view_show(data->parent.win, options, call_active_view_show, call_active_view_hide);
}

static void
call_button_release_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info)
{
	g_debug("release_clicked()");
	ogsmd_call_release(data->id, NULL, NULL);
}


//albacore
static void
call_button_sound_state_clicked(struct CallIncomingViewData *data, Evas_Object *obj, void *event_info)
{
	CallSoundMode mode = call_common_get_sound_mode();
	g_debug("sound_mode_clicked(id=%d,mode=%d)", data->parent.id, mode);

	/* if it's not the first call, do nothing because the ringtone is already off
	 Wait elementary disabled support, so the button 'll not active therefore the question will not take place any more to be*/
	if(call_common_get_active_calls_list()==NULL)
	{		
		if(mode == CALL_SOUND_MODE_SILENT) {
			call_common_set_sound_mode(CALL_SOUND_MODE_ACTIVE);		
		}
		else if (mode == CALL_SOUND_MODE_ACTIVE){
			call_common_set_sound_mode(CALL_SOUND_MODE_SILENT);		
		}
		/* else if moree.... */
		else {
			g_debug("trying to set unknown sound mode %d failed!", mode);
		}
		call_common_window_update_mode(data, call_common_get_sound_mode());
	}
}
//end albacore
