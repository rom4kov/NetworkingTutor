#define _POSIX_C_SOURCE 200809L
#include "data_access_layer.h"
#include <ncurses.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *create_database(WINDOW *win)
{
    sqlite3 *db;
    int rc = sqlite3_open("src/data/networking_tutor.db", &db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // const char *zSql = "CREATE TABLE IF NOT EXISTS users (id int NOT NULL, "
    //                    "name varchar(255) NOT NULL, "
    //                    "short_desc varchar(255), PRIMARY KEY (id))";
    //
    // sqlite3_stmt *stmt;
    //
    // rc = sqlite3_prepare_v2(db, zSql, -1, &stmt, 0);
    //
    // if (rc != SQLITE_OK)
    // {
    //     mvwprintw(win, 1, 2, "Prepare Failed: %s", sqlite3_errmsg(db));
    //     sqlite3_close(db);
    // }
    //
    // rc = sqlite3_step(stmt);
    //
    // if (rc != SQLITE_OK)
    // {
    //     mvwprintw(win, 1, 2, "Step Failed: %s", sqlite3_errmsg(db));
    //     sqlite3_close(db);
    // }

    wrefresh(win);

    return db;
}

USER_DATA get_user_data(sqlite3 *db)
{

    const char *zSql3 = "SELECT * FROM users WHERE id = 1;";

    USER_DATA user_data;

    sqlite3_stmt *stmt3;
    sqlite3_prepare_v2(db, zSql3, -1, &stmt3, NULL);
    sqlite3_step(stmt3);

    user_data.name = sqlite3_column_text(stmt3, 1);
    user_data.language = sqlite3_column_text(stmt3, 2);

    return user_data;
}

void seed_courses_data(sqlite3 *db, WINDOW *win)
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS courses (id INTEGER PRIMARY KEY, name "
        "varchar(255) NOT NULL, short_desc varchar(255));"
        "INSERT OR IGNORE INTO courses (name, short_desc) VALUES('Build an "
        "HTTP Server in C', "
        "'In this first and short project you will learn how to build a simple "
        "HTTP "
        "server in C, using the very basic version 1.0 of the HTTP protocol.');"
        "INSERT OR IGNORE INTO courses (name, short_desc) VALUES('Handling "
        "multiple clients', "
        "'In the second project you will learn how to build a simple HTTP "
        "server in C, using the very basic version 1.0 of the HTTP protocol.');"
        "INSERT OR IGNORE INTO courses (name, short_desc) VALUES('Secure "
        "communication with "
        "SSL', 'In the second project you will learn how to build a simple "
        "HTTP server in C, "
        "using the very basic version 1.0 of the HTTP protocol.');";

    char *err_msg = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        mvwprintw(win, 2, 2, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    else
    {
        mvwprintw(win, 2, 2, "Table courses created successfully\n");
    }
}

COURSE *get_course_data(sqlite3 *db)
{
    int rc = 0;

    const char *sql = "SELECT * FROM courses;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    int num_courses = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        num_courses++;
    }
    sqlite3_reset(stmt);

    COURSE *course_data = malloc(sizeof(COURSE) * num_courses);

    int i = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *description = sqlite3_column_text(stmt, 2);

        course_data[i].id = id;
        course_data[i].name = strdup((const char *)name);
        course_data[i].short_desc = strdup((const char *)description);
        i++;
    }

    sqlite3_finalize(stmt);
    return course_data;
}

void update_user(sqlite3 *db, int id, char *name, char *language)
{
    int rc = 0;

    // char *err_msg = 0;
    sqlite3_stmt *res;

    const char *sql = "UPDATE users SET name = ?, language = ? WHERE id = ?";

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_text(res, 1, name, strlen(name), NULL);
        sqlite3_bind_text(res, 2, language, strlen(language), NULL);
        sqlite3_bind_int(res, 3, id);
    }
    else
    {
        fprintf(stderr, "Failed to execute statement: %s\n",
                sqlite3_errmsg(db));
    }

    sqlite3_step(res);
}
