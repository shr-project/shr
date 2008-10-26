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

#include "contacts-phonegui.h"

void phonegui_connect() {
    phonegui_init = phonegui_get_function("phonegui_init");
    phonegui_contacts_show = phonegui_get_function("phonegui_contacts_show");
    phonegui_contacts_hide = phonegui_get_function("phonegui_contacts_hide");
}

