/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Didier "Ptitjes" <ptitjes@free.fr>
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

#include "ophonekitd-phonelog.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sqlite3.h>

#define PHONELOG_DB_LOCATION "/var/db/phonelog.db"
#define PHONELOG_CREATE_CALL_IDS_TABLE \
	"CREATE TABLE call_ids (id INTEGER PRIMARY KEY, number TEXT)"
#define PHONELOG_CREATE_CALL_EVENTS_TABLE \
	"CREATE TABLE call_events (id INTEGER, status INT, eventTime TIMESTAMP, FOREIGN KEY (id) REFERENCES call_ids(id))"
#define PHONELOG_CREATE_CALL_EVENTS_TRIGGER \
	"CREATE TRIGGER insert_call_events_eventTime AFTER  INSERT ON call_events\nBEGIN\nUPDATE call_events SET eventTime = DATETIME('NOW')  WHERE rowid = new.rowid;\nEND"

sqlite3 *db;

void phonelog_init_database() {
	int rc;
	char *err = 0;
	struct stat info;
	int dbIsNew;

	/* Test whether the database file exists */
	dbIsNew = stat(PHONELOG_DB_LOCATION, &info) != 0;

	if (dbIsNew) {
		g_debug("phonelog - creating database tables and triggers");
	}

	/* Open the database */
	rc = sqlite3_open(PHONELOG_DB_LOCATION, &db);
	if (rc) {
		g_debug("phonelog - can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		db = 0;
		return;
	}

	/* Create the call_events table and trigger if the database is new */
	if (dbIsNew) {
		rc = sqlite3_exec(db, PHONELOG_CREATE_CALL_IDS_TABLE, 0, 0, &err);
		if (rc != SQLITE_OK) {
			g_debug("phonelog - SQL error: %s\n", err);
			sqlite3_free(&err);

			sqlite3_close(db);
			db = 0;
			return;
		}

		rc = sqlite3_exec(db, PHONELOG_CREATE_CALL_EVENTS_TABLE, 0, 0, &err);
		if (rc != SQLITE_OK) {
			g_debug("phonelog - SQL error: %s\n", err);
			sqlite3_free(&err);

			sqlite3_close(db);
			db = 0;
			return;
		}

		rc = sqlite3_exec(db, PHONELOG_CREATE_CALL_EVENTS_TRIGGER, 0, 0, &err);
		if (rc != SQLITE_OK) {
			g_debug("phonelog - SQL error: %s\n", err);
			sqlite3_free(&err);

			sqlite3_close(db);
			db = 0;
			return;
		}
	}
}

void phonelog_close_database() {
	/* Close the database */
	if (db) {
		sqlite3_close(db);
	}
}

int phonelog_add_new_call(const gchar* number) {
	int rc;
	char *err = 0;
	char *stmt = 0;

	if (db) {
		g_debug("phonelog - add new call, number: %s", number);

		stmt = malloc(51 + strlen(number) + 1);
		sprintf(stmt, "INSERT INTO call_ids (id, number) values (NULL, %s)",
				number);
		g_debug(stmt);

		rc = sqlite3_exec(db, stmt, 0, 0, &err);
		free(stmt);

		if (rc != SQLITE_OK) {
			g_debug("phonelog - SQL error: %s\n", err);
			sqlite3_free(&err);
			return -1;
		}

		return sqlite3_last_insert_rowid(db);
	}
}

void phonelog_log_call_event(const int unique_id, const int status) {
	int rc;
	char *err = 0;
	char *stmt = 0;

	g_debug("phonelog - logging call event, unique id: %u, status: %d",
			unique_id, status);

	if (db) {
		stmt = malloc(48 + 100 + 1);
		sprintf(stmt, "INSERT INTO call_events (id, status) values (%u, %u)",
				unique_id, status);
		g_debug(stmt);

		rc = sqlite3_exec(db, stmt, 0, 0, &err);
		free(stmt);

		if (rc != SQLITE_OK) {
			g_debug("phonelog - SQL error: %s\n", err);
			sqlite3_free(&err);
		}
	}
}
