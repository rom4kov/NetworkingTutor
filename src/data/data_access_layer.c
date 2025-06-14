#define _POSIX_C_SOURCE 200809L
#include "data_access_layer.h"
#include <ncurses.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *create_database()
{
    sqlite3 *db;
    int rc = sqlite3_open("src/data/networking_tutor.db", &db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    return db;
}

char *read_sql_query(char *filename)
{
    char c;
    int i = 0;

    FILE *sql_query_file = fopen(filename, "r+");
    if (sql_query_file == NULL)
    {
        perror("file");
    }

    fseek(sql_query_file, 0, SEEK_END);
    int file_size = ftell(sql_query_file);
    rewind(sql_query_file);

    char *query_string = calloc(file_size, sizeof(char));

    while ((fread(&c, sizeof(char), 1, sql_query_file))) 
    {
        if (c == '\n')
        {
            query_string[i] = ' ';
        }
        else if (c == '@')
        {
            query_string[i] = '\n';
        }
        else 
        {
            query_string[i] = c;
        }
        i++;
    }

    query_string[i + 1] = '\0';

    return query_string;
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
    const char *course_details = read_sql_query("SQL/courses/http_server/intro/0_intro.sql");

    char *err_msg = 0;

    int rc = sqlite3_exec(db, course_details, 0, 0, &err_msg);

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

COURSE_SECTION *get_course_section_data(sqlite3 *db, int active_course,
                                       int section, int *num_of_items)
{
    int rc = 0;

    const char *sql =
        "SELECT * FROM materials WHERE course_id = ? AND section_id = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, active_course);
        sqlite3_bind_int(stmt, 2, section);
    }

    int num_steps = 1;
    // if (sqlite3_step(stmt) != SQLITE_ROW)
    // {
    //     return NULL;
    // }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        num_steps++;
    }
    sqlite3_reset(stmt);

    COURSE_SECTION *course_section_data = malloc(sizeof(COURSE_SECTION) * num_steps);

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const int id = sqlite3_column_int(stmt, 1);
        const unsigned char *section_title = sqlite3_column_text(stmt, 2);
        const int section_id = sqlite3_column_int(stmt, 3);
        const unsigned char *content_title = sqlite3_column_text(stmt, 4);
        const unsigned char *content = sqlite3_column_text(stmt, 5);
        const int order_num = sqlite3_column_int(stmt, 6);

        course_section_data[i].id = id;
        course_section_data[i].section_title = strdup((const char *)section_title);
        course_section_data[i].section_id = section_id;
        course_section_data[i].content_title = strdup((const char *)content_title);
        course_section_data[i].content = strdup((const char *)content);
        course_section_data[i].order_num = order_num;
        i++;
    }

    *num_of_items = i;

    sqlite3_finalize(stmt);
    return course_section_data;
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
