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

#ifndef _FRAMEWORKD_PHONEGUI_H
#define _FRAMEWORKD_PHONEGUI_H

#define FRAMEWORKD_PHONEGUI_CONFIG "/etc/frameworkd-phonegui.conf"

typedef enum {
    PHONEGUI_DIALOG_ERROR_DO_NOT_USE,  
        // This value is used for checking if we get a wrong pointer out of a HashTable. 
        // So do not use it, and leave it first in this enum. ( because 0 == NULL )
    PHONEGUI_DIALOG_MESSAGE_STORAGE_FULL,
    PHONEGUI_DIALOG_SIM_NOT_PRESENT
} PhoneguiDialogType;

void phonegui_load(const char *application_name);
void* phonegui_get_function(const char *function_name);

#endif
