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
#define PHONELOG_DATABASE_SQL_FILE "/usr/share/ophonekitd/phonelog-database.sql"

#define PHONELOG_INSERT_CALL \
    "INSERT INTO call_ids (id, number) values (NULL, ?)"
#define PHONELOG_INSERT_CALL_EVENT \
    "INSERT INTO call_events (id, status) values (?, ?)"

sqlite3 *db;
sqlite3_stmt *insert_call;
sqlite3_stmt *insert_call_event;

int read_file(const char *name, char **content) {
    FILE *file;
    long size;
    size_t result;

    file = fopen(name, "rb");
    if (file == NULL) {
        g_debug("phonelog - file error while opening %s", name);
        return 1;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    /* Allocate memory to contain the whole file */
    *content = (char*) malloc(sizeof(char) * size);
    if (*content == NULL) {
        g_debug("phonelog - memory error while reading %s", name);
        return 1;
    }

    /* Copy the file into the buffer */
    result = fread(*content, 1, size, file);
    if (result != size) {
        g_debug("phonelog - file error while reading %s", name);
        return 1;
    }

    fclose(file);
    return 0;
}

void debug_io_error(const char* activity, int fatal) {
    if (fatal) {
        g_warning("phonelog - error while %s\n", activity);
        sqlite3_close(db);
        db = 0;
    } else {
        g_debug("phonelog - error while %s\n", activity);
    }
}

void debug_sqlite3_error(const char* activity, const int rc, int fatal) {
    const char *err = sqlite3_errmsg(db);

    if (fatal) {
        g_warning("phonelog - error while %s: %s (code=%d)\n", activity, err, rc);
        sqlite3_close(db);
        db = 0;
    } else {
        g_debug("phonelog - error while %s: %s (code=%d)\n", activity, err, rc);
    }
}

void phonelog_init_database() {
    int rc;
    struct stat info;
    int dbIsNew;

    /* Test whether the database file exists */
    dbIsNew = stat(PHONELOG_DB_LOCATION, &info) != 0;

    /* Open the database */
    if ((rc = sqlite3_open(PHONELOG_DB_LOCATION, &db)) != SQLITE_OK) {
        debug_sqlite3_error("opening database", rc, 1);
        return;
    }

    /* Create the call_events table and trigger if the database is new */
    if (dbIsNew) {
        char *sql;
        char *err = 0;

        g_debug("phonelog - initiating database tables, triggers and views");

        if (read_file(PHONELOG_DATABASE_SQL_FILE, &sql)) {
            debug_io_error("reading database SQL file", 1);
            return;
        }

        if ((rc = sqlite3_exec(db, sql, 0, 0, &err)) != SQLITE_OK) {
            debug_sqlite3_error("executing SQL file", rc, 1);
            return;
        }
    }

    /* Prepare insert statements once and for all */
    if (db) {
        const char *pzTail;

        if ((rc = sqlite3_prepare_v2(db, PHONELOG_INSERT_CALL, -1,
                        &insert_call, &pzTail)) != SQLITE_OK) {
            debug_sqlite3_error("preparing insert call statement", rc, 1);
            return;
        }

        if ((rc = sqlite3_prepare_v2(db, PHONELOG_INSERT_CALL_EVENT, -1,
                        &insert_call_event, &pzTail)) != SQLITE_OK) {
            debug_sqlite3_error("preparing insert call event statement", rc, 1);
            return;
        }
    }
}

void phonelog_close_database() {
    /* Finalize insert statements */
    if (insert_call) {
        sqlite3_finalize(insert_call);
    }
    if (insert_call_event) {
        sqlite3_finalize(insert_call_event);
    }

    /* Close the database */
    if (db) {
        sqlite3_close(db);
    }
}

int phonelog_add_new_call(const gchar* number) {
    int rc;

    if (db) {
        g_debug("phonelog - add new call, number: %s", number);

        if (((rc = sqlite3_reset(insert_call)) == SQLITE_OK) &&
                ((rc = sqlite3_bind_text(insert_call, 1, number, -1, SQLITE_TRANSIENT)) == SQLITE_OK) &&
                ((rc = sqlite3_step(insert_call)) == SQLITE_DONE))
            return sqlite3_last_insert_rowid(db);
        debug_sqlite3_error("inserting call", rc, 0);
    }
    return -1;
}

void phonelog_log_call_event(const int unique_id, const int status) {
    int rc;

    if (db) {
        g_debug("phonelog - logging call event, unique id: %u, status: %u",
                unique_id, status);

        if (((rc = sqlite3_reset(insert_call_event)) != SQLITE_OK) ||
                ((rc = sqlite3_bind_int(insert_call_event, 1, unique_id)) != SQLITE_OK) ||
                ((rc = sqlite3_bind_int(insert_call_event, 2, status)) != SQLITE_OK) ||
                ((rc = sqlite3_step(insert_call_event)) != SQLITE_DONE))
            debug_sqlite3_error("inserting call event", rc, 0);
    }
}
