#include "views.h"
#include "call-common.h"


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

void
call_button_release_clicked(struct CallViewData *data, Evas_Object *obj, void *event_info)
{
	g_debug("release_clicked()");
	ogsmd_call_release(data->id, NULL, NULL);
}
