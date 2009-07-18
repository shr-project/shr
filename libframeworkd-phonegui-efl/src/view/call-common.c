#include "views.h"
#include "call-common.h"

struct _CallsList {
	struct {
		int id;
		struct CallActiveViewData *win;
	} *list;
	unsigned int len;
};

/* FIXME: add locks to all the active calls functions!!!
 * atm no call comes back up automatically, fix that! 
 */
static CallsList active_calls_list = { NULL, 0 } ;

void
call_common_window_new_active(int id)
{
	g_debug("%s:%d refreshing calls list\n", __FILE__, __LINE__);
	if (active_calls_list.len) {
		int i;
		for (i = 0 ; i < active_calls_list.len ; i++) {
			if (id != active_calls_list.list[i].id) {
				call_common_window_to_pending(active_calls_list.list[i].win);
			}
			else {
				call_common_window_to_active(active_calls_list.list[i].win);
			}
		}
	}
}

void
call_common_window_to_pending(struct CallActiveViewData *win)
{
	if (win->state == CALL_STATE_ACTIVE) {
		elm_button_label_set(win->bt1, D_("Pickup"));
	}
	else if (win->state == CALL_STATE_PENDING) {
		/*Do nothing as we want it to be pending*/
		g_debug("Found a pending call while expecting none!");
	}
	else {
		g_debug("Bad state (%d) for an active call!", win->state);
	}
	win->state = CALL_STATE_PENDING;
}

void
call_common_window_to_active(struct CallActiveViewData *win)
{
	if (win->state == CALL_STATE_ACTIVE) {
		/*Do nothing as we want it to be active*/
		g_debug("Found an active call while expecting none!");
	}
	else if (win->state == CALL_STATE_PENDING) {
		elm_button_label_set(win->bt1, D_("Release"));
	}
	else {
		g_debug("Bad state (%d) for an active call!", win->state);
	}
	win->state = CALL_STATE_ACTIVE;
}

int
call_common_active_call_add(struct CallActiveViewData *win)
{
	g_debug("%s:%d adding a call to list (%id)", __FILE__, __LINE__, win->parent.id);
	/* if it's not the first call, update all the windows */
	if (active_calls_list.len) {
		int i;
		for (i = 0 ; i < active_calls_list.len ; i++) {
			call_common_window_to_pending(active_calls_list.list[i].win);
		}
	}
		
	active_calls_list.list = realloc(active_calls_list.list,
	                                 (active_calls_list.len + 1) * sizeof(CallsList));
	active_calls_list.list[active_calls_list.len].id = win->parent.id;
	active_calls_list.list[active_calls_list.len].win = win;
	active_calls_list.len++;
	return 0;		
}

int
call_common_active_call_remove(int id)
{
	g_debug("%s:%d removing a call from list (id:%d)", __FILE__, __LINE__, id);
	int i = active_calls_list.len - 1;
	while (i >= 0 && active_calls_list.list[i].id != id)
		i--;
	/* if we haven't found abort */
	if (i < 0)
		return 1;
	
	/* put last instead of it */
	active_calls_list.list[i].id = active_calls_list.list[active_calls_list.len - 1].id;
	active_calls_list.list[i].win = active_calls_list.list[active_calls_list.len - 1].win;
	active_calls_list.list = realloc(active_calls_list.list,
	                                 (active_calls_list.len - 1) * sizeof(CallsList));
	active_calls_list.len--;
	
	/* if there's only one call left, update windows */
	if (active_calls_list.len == 1) 
		call_common_window_to_active(active_calls_list.list[0].win);
	return 0;
}

struct CallActiveViewData *
call_common_active_calls_get_window(int id)
{
	int i = active_calls_list.len;
	while (i >= 0 && active_calls_list.list[i].id != id)
		i--;

	if (i < 0)
		return NULL;

	return active_calls_list.list[i].win;
}

/*FIXME: why does button_keypad_clicked get the data parametr? */
void
call_button_keypad_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info)
{
	char string[2];
	string[0] = (char) event_info;
	string[1] = '\0';
	g_debug("call_button_keypad_clicked(): %s", string);
	ogsmd_call_send_dtmf(strdup(string), NULL, NULL);
}

void
call_dtmf_enable(struct CallViewData *data)
{
	g_debug("call_dtmf_enable()");
	data->keypad = elm_keypad_add(window_evas_object_get(data->win));
	evas_object_smart_callback_add(data->keypad, "clicked", call_button_keypad_clicked, data);
	window_swallow(data->win, "keypad", data->keypad);
	evas_object_show(data->keypad);
}

void
call_dtmf_disable(struct CallViewData *data)
{
	g_debug("call_dtmf_disable()");
	evas_object_smart_callback_del(data->keypad, "clicked", call_button_keypad_clicked);
	evas_object_del(data->keypad);
}

void
call_speaker_enable()
{
	g_debug("call_speaker_enable()");
	odeviced_audio_pull_scenario(NULL, NULL);
}

void
call_speaker_disable()
{
	g_debug("call_speaker_disable()");
	odeviced_audio_push_scenario("gsmspeakerout", NULL, NULL);
}
