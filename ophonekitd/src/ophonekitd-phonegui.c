/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              quickdev
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 */

#include "ophonekitd-phonegui.h"

void phonegui_connect() {
    phonegui_init = phonegui_get_function("phonegui_init");
    phonegui_sim_auth_ui_show = phonegui_get_function("phonegui_sim_auth_ui_show");
    phonegui_sim_auth_ui_hide = phonegui_get_function("phonegui_sim_auth_ui_hide");
    phonegui_incoming_call_ui_show = phonegui_get_function("phonegui_incoming_call_ui_show");
    phonegui_incoming_call_ui_hide = phonegui_get_function("phonegui_incoming_call_ui_hide");
    phonegui_outgoing_call_ui_show = phonegui_get_function("phonegui_outgoing_call_ui_show");
    phonegui_outgoing_call_ui_hide = phonegui_get_function("phonegui_outgoing_call_ui_hide");
    phonegui_message_ui_show = phonegui_get_function("phonegui_message_ui_show");
    phonegui_message_ui_hide = phonegui_get_function("phonegui_message_ui_hide");
}

