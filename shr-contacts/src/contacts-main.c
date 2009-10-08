/*
 *  Copyright (C) 2007 Openmoko, Inc.
 *
 *  Authored by:
 *    quickdev
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

#include <frameworkd-phonegui/frameworkd-phonegui.h>


int
main(int argc, char **argv)
{
	/* Load, connect and initiate phonegui */
	phonegui_load("contacts");
	phonegui_init(argc, argv, NULL);

	phonegui_contacts_show();

	return 0;
}

