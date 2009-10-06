#include "views.h"


struct DialerViewData {
	struct Window *win;
	char number[65];
	Evas_Object *keypad, *bt_options, *bt_call, *bt_exit, *hv, *bx,
		*bt_save, *bt_message;
	Evas_Object *text_number, *text_number_info, *delete_text_icon,
		*delete_text_button;
	Ecore_Timer *delete_timer;
};


static void frame_dialer_show(struct DialerViewData *data);
static void frame_dialer_hide(struct DialerViewData *data);
static void frame_dialer_options_clicked(struct DialerViewData *data,
					 Evas_Object * obj, void *event_info);
static void frame_dialer_keypad_clicked(struct DialerViewData *data,
					Evas_Object * obj, void *event_info);
static void frame_dialer_call_clicked(struct DialerViewData *data,
				      Evas_Object * obj, void *event_info);
static void frame_dialer_exit_clicked(struct DialerViewData *data,
				      Evas_Object * obj, void *event_info);
static void frame_dialer_save_clicked(struct DialerViewData *data,
				      Evas_Object * obj, void *event_info);
static void frame_dialer_message_clicked(struct DialerViewData *data,
					 Evas_Object * obj, void *event_info);
static void frame_dialer_delete_mouse_down(struct DialerViewData *data,
					   Evas_Object * o,
					   const char *emission,
					   const char *source);
static void frame_dialer_delete(struct DialerViewData *data, Evas_Object * o,
				const char *emission, const char *source);
static void frame_dialer_number_clicked(struct DialerViewData *data,
					Evas_Object * o, const char *emission,
					const char *source);
static void frame_dialer_number_clear(struct DialerViewData *data);
static void frame_dialer_number_update(struct DialerViewData *data);
static void frame_dialer_initiate_callback(GError * error, int call_id,
					   void *userdata);
static void frame_dialer_initiate_callback2(struct DialerViewData *data);



struct DialerViewData *
dialer_view_show(struct Window *win, GHashTable * options)
{
	struct DialerViewData *data =
		g_slice_alloc0(sizeof(struct DialerViewData));
	data->win = win;
	data->number[0] = '\0';
	data->delete_timer = NULL;

	window_frame_show(win, data, frame_dialer_show, frame_dialer_hide);
	window_show(win);
	return data;
}

void
dialer_view_hide(struct DialerViewData *data)
{
	g_slice_free(struct DialerViewData, data);
}


/*
 * Frame "dialer"
 */

static void
frame_dialer_show(struct DialerViewData *data)
{
	struct Window *win = data->win;
	window_layout_set(win, DIALER_FILE, "main");

	data->text_number = elm_label_add(window_evas_object_get(win));
	elm_label_label_set(data->text_number, "");
	evas_object_size_hint_align_set(data->text_number, 0.0, 0.5);
	window_swallow(win, "text_number", data->text_number);
	evas_object_show(data->text_number);

	data->text_number_info = elm_label_add(window_evas_object_get(win));
	elm_label_label_set(data->text_number_info,
			    D_("Click to open contactlist."));
	window_swallow(win, "text_number_info", data->text_number_info);
	evas_object_show(data->text_number_info);

	data->delete_text_icon = elm_icon_add(window_evas_object_get(win));
	evas_object_size_hint_aspect_set(data->delete_text_icon,
					 EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
	elm_icon_file_set(data->delete_text_icon, DELETE_TEXT_ICON, NULL);

	data->delete_text_button = elm_button_add(window_evas_object_get(win));
	elm_button_icon_set(data->delete_text_button, data->delete_text_icon);
	evas_object_smart_callback_add(data->delete_text_button, "clicked",
				       frame_dialer_delete, data);

	window_swallow(win, "button_delete", data->delete_text_button);
	evas_object_show(data->delete_text_button);
	evas_object_show(data->delete_text_icon);


	data->keypad = elm_keypad_add(window_evas_object_get(win));
	evas_object_smart_callback_add(data->keypad, "clicked",
				       frame_dialer_keypad_clicked, data);
	window_swallow(win, "keypad", data->keypad);
	evas_object_show(data->keypad);

	data->bt_exit = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_exit, D_("Close"));
	evas_object_smart_callback_add(data->bt_exit, "clicked",
				       frame_dialer_exit_clicked, data);
	window_swallow(win, "button_exit", data->bt_exit);
	evas_object_show(data->bt_exit);

	data->bt_options = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_options, D_("More"));
	evas_object_smart_callback_add(data->bt_options, "clicked",
				       frame_dialer_options_clicked, data);
	window_swallow(win, "button_options", data->bt_options);
	evas_object_show(data->bt_options);

	data->bt_call = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_call, D_("Call"));
	evas_object_smart_callback_add(data->bt_call, "clicked",
				       frame_dialer_call_clicked, data);
	window_swallow(win, "button_call", data->bt_call);
	evas_object_show(data->bt_call);

	edje_object_signal_callback_add(elm_layout_edje_get
					(window_layout_get(win)), "click",
					"number", frame_dialer_number_clicked,
					data);
	edje_object_signal_callback_add(elm_layout_edje_get
					(window_layout_get(win)), "mouse_down",
					"delete",
					frame_dialer_delete_mouse_down, data);

	/* Options */
	data->hv = elm_hover_add(window_evas_object_get(win));
	elm_hover_parent_set(data->hv, window_evas_object_get(win));
	elm_hover_target_set(data->hv, data->bt_options);

	data->bx = elm_box_add(window_evas_object_get(win));
	elm_box_horizontal_set(data->bx, 0);
	elm_box_homogenous_set(data->bx, 1);
	evas_object_show(data->bx);

	data->bt_save = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_save, D_("Save"));
	evas_object_size_hint_min_set(data->bt_save, 130, 80);
	evas_object_smart_callback_add(data->bt_save, "clicked",
				       frame_dialer_save_clicked, data);
	evas_object_show(data->bt_save);
	elm_box_pack_end(data->bx, data->bt_save);

	data->bt_message = elm_button_add(window_evas_object_get(win));
	elm_button_label_set(data->bt_message, D_("Send SMS"));
	evas_object_size_hint_min_set(data->bt_message, 130, 80);
	evas_object_smart_callback_add(data->bt_message, "clicked",
				       frame_dialer_message_clicked, data);
	evas_object_show(data->bt_message);
	elm_box_pack_end(data->bx, data->bt_message);

	elm_hover_content_set(data->hv, "top", data->bx);
}

static void
frame_dialer_hide(struct DialerViewData *data)
{
	struct Window *win = data->win;

	evas_object_smart_callback_del(data->keypad, "clicked",
				       frame_dialer_keypad_clicked);
	evas_object_del(data->keypad);
	evas_object_del(data->bt_options);
	evas_object_del(data->bt_call);
	evas_object_del(data->bt_exit);
	evas_object_del(data->bt_message);
	evas_object_del(data->bt_save);
	evas_object_del(data->bx);
	evas_object_del(data->hv);
	evas_object_del(data->text_number);
	evas_object_del(data->text_number_info);
	evas_object_del(data->delete_text_button);
}

static void
frame_dialer_options_clicked(struct DialerViewData *data, Evas_Object * obj,
			     void *event_info)
{
	evas_object_show(data->hv);
}

static void
frame_dialer_exit_clicked(struct DialerViewData *data, Evas_Object * obj,
			  void *event_info)
{
	window_destroy(data->win, NULL);
}

static void
frame_dialer_save_clicked(struct DialerViewData *data, Evas_Object * obj,
			  void *event_info)
{
	GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(options, "number", data->number);

	struct Window *win = window_new(D_("Add Contact"));
	window_init(win);
	window_view_show(win, options, contact_edit_view_show,
			 contact_edit_view_hide);

	window_destroy(data->win, NULL);
}

static void
frame_dialer_call_clicked(struct DialerViewData *data, Evas_Object * obj,
			  void *event_info)
{
	if (strlen(data->number)) {
		if ((data->number[strlen(data->number) - 1] == '#')
		    || (strlen(data->number) <= 2)) {
			g_debug("USSD Request");
			ogsmd_network_send_ussd_request(data->number, NULL,
							NULL);
			//Clean number in dialer
			data->number[0] = '\0';
			frame_dialer_number_update(data);
			edje_object_signal_emit(elm_layout_edje_get
						(window_layout_get(data->win)),
						"number_empty", "elm");
		}
		else {
			g_debug("Initiate Call");
			ogsmd_call_initiate(data->number, "voice",
					    frame_dialer_initiate_callback,
					    data);
		}
	}
}

static void
frame_dialer_message_clicked(struct DialerViewData *data, Evas_Object * obj,
			     void *event_info)
{
	GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(options, "recipient", data->number);

	struct Window *win = window_new(D_("Compose SMS"));
	window_init(win);
	window_view_show(win, options, message_new_view_show,
			 message_new_view_hide);

	window_destroy(data->win, NULL);
}

static void
frame_dialer_keypad_clicked(struct DialerViewData *data, Evas_Object * obj,
			    void *event_info)
{
	char input = (char) event_info;
	int length = strlen(data->number);

	if (length < 64) {
		data->number[length] = input;
		data->number[length + 1] = '\0';
		frame_dialer_number_update(data);
	}

	if (length == 0) {
		edje_object_signal_emit(elm_layout_edje_get
					(window_layout_get(data->win)),
					"number_available", "elm");
	}
}

static void
frame_dialer_delete(struct DialerViewData *data, Evas_Object * o,
		    const char *emission, const char *source)
{
	int length = strlen(data->number);

	if (data->delete_timer != NULL) {
		ecore_timer_del(data->delete_timer);
		data->delete_timer = NULL;
	}

	if (length != 0) {
		data->number[length - 1] = '\0';
		frame_dialer_number_update(data);
		length--;
	}

	if (length == 0) {
		edje_object_signal_emit(elm_layout_edje_get
					(window_layout_get(data->win)),
					"number_empty", "elm");
	}
}


static void
frame_dialer_delete_mouse_down(struct DialerViewData *data, Evas_Object * o,
			       const char *emission, const char *source)
{
	if (data->delete_timer == NULL) {
		data->delete_timer =
			ecore_timer_add(0.5, frame_dialer_number_clear, data);
	}
}

static void
frame_dialer_number_clicked(struct DialerViewData *data, Evas_Object * o,
			    const char *emission, const char *source)
{
	if (!strlen(data->number)) {
		struct Window *win = window_new(D_("Contacts"));
		window_init(win);
		window_view_show(win, NULL, contact_list_view_show,
				 contact_list_view_hide);

		window_destroy(data->win, NULL);
	}
}

static void
frame_dialer_number_update(struct DialerViewData *data)
{
	int length = strlen(data->number);
	char *number = data->number;
	static char tmp[73];

	if (length < 7)
		elm_object_scale_set(data->text_number, 4.0);
	else if (length < 9)
		elm_object_scale_set(data->text_number, 3.0);
	else if (length < 24) {
		elm_object_scale_set(data->text_number, 2.0);
		if (length > 12) {
			tmp[0] = 0;
			strncat(tmp, number, 12);
			strcat(tmp, "<br>");
			strcat(tmp, number + 12);
			number = tmp;
		}
	}
	else {
		elm_object_scale_set(data->text_number, 1.0);
		if (length > 52) {
			tmp[0] = 0;
			strncat(tmp, number, 26);
			strcat(tmp, "<br>");
			strncat(tmp, number + 26, 26);
			strcat(tmp, "<br>");
			strcat(tmp, number + 52);
			number = tmp;
		}
		else if (length > 26) {
			tmp[0] = 0;
			strncat(tmp, number, 26);
			strcat(tmp, "<br>");
			strcat(tmp, number + 26);
			number = tmp;
		}
	}

	elm_label_label_set(data->text_number, number);
}


static void
frame_dialer_number_clear(struct DialerViewData *data)
{
	if (data->delete_timer != NULL) {
		ecore_timer_del(data->delete_timer);
		data->delete_timer = NULL;

		data->number[0] = '\0';
		frame_dialer_number_update(data);

		edje_object_signal_emit(elm_layout_edje_get
					(window_layout_get(data->win)),
					"number_empty", "elm");
	}
}

static void
frame_dialer_initiate_callback(GError * error, int call_id, void *userdata)
{
	async_trigger(frame_dialer_initiate_callback2, userdata);
}

static void
frame_dialer_initiate_callback2(struct DialerViewData *data)
{
	window_destroy(data->win, NULL);
}
