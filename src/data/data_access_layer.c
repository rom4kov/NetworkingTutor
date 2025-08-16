#define _POSIX_C_SOURCE 200809L
#include "data_access_layer.h"
#include "../core/core.h"
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

    char *query_string = calloc(file_size + 1, sizeof(char));
    bool custom_line_break = false;

    while ((fread(&c, sizeof(char), 1, sql_query_file)))
    {
        if (c == '\n')
        {
            if (custom_line_break)
            {
                custom_line_break = false;
                continue;
            }
            query_string[i] = ' ';
        }
        else if (c == '@')
        {
            query_string[i] = '\n';
            custom_line_break = true;
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

int create_new_user(APP_CONTEXT *ctx, char *username)
{
    int rc = 0;

    const char *sql = "INSERT INTO users (name, created_at) VALUES (?, ?);";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        char *datetime = current_datetime();
        sqlite3_bind_text(stmt, 1, username, strlen(username), NULL);
        sqlite3_bind_text(stmt, 2, datetime, strlen(datetime), NULL);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        mvwprintw(ctx->greeter_screen, 6, 2, "Insert failed: %s",
                  sqlite3_errmsg(ctx->db));
    }
    else
    {
        mvwprintw(ctx->greeter_screen, 6, 2, "%s", "Insert successful");
    }

    return (int)sqlite3_last_insert_rowid(ctx->db);
}

USER_DATA *get_user_data(sqlite3 *db, int user_id)
{
    const char *zSql = "SELECT * FROM users WHERE id = ?;";

    USER_DATA *user_data = malloc(sizeof(USER_DATA));

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, zSql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, user_id);
    }
    sqlite3_step(stmt);

    user_data->name = sqlite3_column_text(stmt, 1);
    user_data->created_at = sqlite3_column_text(stmt, 2);

    // sqlite3_finalize(stmt);

    return user_data;
}

void seed_courses_data(sqlite3 *db, WINDOW *win, char *query)
{
    const char *course_details = read_sql_query(query);

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

COURSE *get_course_by_id(sqlite3 *db, int course_id)
{
    COURSE *course = malloc(sizeof(COURSE));

    const char *sql = "SELECT * FROM courses WHERE id = ?;";

    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, course_id);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *description = sqlite3_column_text(stmt, 2);
        course->id = id;
        course->name = strdup((const char *)name);
        course->short_desc = strdup((const char *)description);
    }

    return course;
}

SECTION_METADATA *get_section_metadata(APP_CONTEXT *ctx)
{
    SECTION_METADATA *section_metadata = malloc(sizeof(SECTION_METADATA));

    int rc = 0;

    const char *sql = "SELECT section_title, has_test FROM sections WHERE "
                      "course_id = ? AND order_num = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_course_id);
        sqlite3_bind_int(stmt, 2, ctx->rp_state->curr_section);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        section_metadata->title = (char *)sqlite3_column_text(stmt, 0);
        section_metadata->has_test = (bool)sqlite3_column_int(stmt, 1);
        section_metadata->has_separate_task = (bool)sqlite3_column_int(stmt, 2);
    }

    return section_metadata;
}

COURSE_SECTION *get_course_section_materials(sqlite3 *db, int course,
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
        sqlite3_bind_int(stmt, 1, course);
        sqlite3_bind_int(stmt, 2, section);
    }

    int num_steps = 1;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        num_steps++;
    }
    sqlite3_reset(stmt);

    COURSE_SECTION *course_section_data =
        malloc(sizeof(COURSE_SECTION) * num_steps);

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const int id = sqlite3_column_int(stmt, 1);
        const int section_id = sqlite3_column_int(stmt, 2);
        const unsigned char *content_title = sqlite3_column_text(stmt, 3);
        const unsigned char *content = sqlite3_column_text(stmt, 4);
        const int order_num = sqlite3_column_int(stmt, 5);
        const bool syntax_hl = sqlite3_column_int(stmt, 6);

        course_section_data[i].id = id;
        course_section_data[i].section_id = section_id;
        course_section_data[i].content_title =
            strdup((const char *)content_title);
        course_section_data[i].content = strdup((const char *)content);
        course_section_data[i].order_num = order_num;
        course_section_data[i].syntax_hl = syntax_hl;
        i++;
    }

    *num_of_items = i;

    sqlite3_finalize(stmt);
    return course_section_data;
}

void get_total_course_sections(APP_CONTEXT *ctx)
{
    int rc = 0;

    const char *sql =
        "SELECT COUNT(DISTINCT section_id) FROM materials WHERE course_id = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_course_id);
    }

    int sections = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        sections = sqlite3_column_int(stmt, 0);
    }

    ctx->rp_state->total_course_sections = sections;
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

void set_section_completed(APP_CONTEXT *ctx)
{
    int rc = 0;

    sqlite3_stmt *res;

    const char *sql = "UPDATE progress SET section_completed = 1 "
                      "WHERE user_id = 1 AND course_id = ? "
                      "AND section_id = ?;";

    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &res, 0);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(res, 1, ctx->current_course_id);
        sqlite3_bind_int(res, 2, ctx->rp_state->curr_section);
    }
    else
    {
        mvwprintw(ctx->course_windows[4], 1, 2, "SQL error: %s\n",
                  sqlite3_errmsg(ctx->db));
    }

    sqlite3_step(res);
}

void get_completed_sections(APP_CONTEXT *ctx)
{
    int rc = 0;

    sqlite3_stmt *stmt;

    const char *sql = "SELECT COUNT(*) FROM progress WHERE user_id = ? AND "
                      "course_id = ? AND section_completed = 1;";

    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_user_id);
        sqlite3_bind_int(stmt, 2, ctx->current_course_id);
    }
    else
    {
        mvwprintw(ctx->course_windows[4], 1, 2, "SQL error: %s\n",
                  sqlite3_errmsg(ctx->db));
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ctx->rp_state->sections_completed = sqlite3_column_int(stmt, 0);
    }
}

int get_total_completed_sections(APP_CONTEXT *ctx)
{
    int rc = 0;

    sqlite3_stmt *stmt;

    const char *sql = "SELECT COUNT(*) FROM progress WHERE user_id = ? AND "
                      "section_completed = 1;";

    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_user_id);
    }
    else
    {
        mvwprintw(ctx->course_windows[4], 1, 2, "SQL error: %s\n",
                  sqlite3_errmsg(ctx->db));
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        return sqlite3_column_int(stmt, 0);
    }
    return 0;
}

void set_items_completed(APP_CONTEXT *ctx)
{
    int rc = 0;

    sqlite3_stmt *res;

    const char *sql;

    int curr_item =
        ctx->rp_state->curr_item - (ctx->rp_state->curr_section == 0 ? 1 : 0);

    sql = "INSERT INTO progress (user_id, course_id, "
          "section_id, items_completed) VALUES (1, ?, ?, ?) "
          "ON CONFLICT(user_id, course_id, section_id) DO UPDATE SET "
          "items_completed = excluded.items_completed;";

    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &res, 0);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(res, 1, ctx->current_course_id);
        sqlite3_bind_int(res, 2, ctx->rp_state->curr_section);
        sqlite3_bind_int(res, 3, curr_item);
        mvwprintw(ctx->course_windows[4], 1, 2, "rc: %i", rc);
    }
    else
    {
        mvwprintw(ctx->course_windows[4], 1, 2, "SQL error: %s\n",
                  sqlite3_errmsg(ctx->db));
    }

    rc = sqlite3_step(res);
}

int get_total_completed_items(APP_CONTEXT *ctx)
{
    int rc = 0;
    int total_items = 0;

    sqlite3_stmt *stmt;

    const char *sql = "SELECT items_completed FROM progress WHERE user_id = ?;";

    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_user_id);
    }
    else
    {
        mvwprintw(ctx->course_windows[4], 1, 2, "SQL error: %s\n",
                  sqlite3_errmsg(ctx->db));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        total_items += sqlite3_column_int(stmt, 0);
    }
    return total_items;
}

int get_current_streak(APP_CONTEXT *ctx)
{

    return 0;
}

int get_current_course(sqlite3 *db, int user_id)
{
    int rc = 0;

    const char *sql = "SELECT max(course_id) FROM progress WHERE user_id = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, user_id);
    }

    sqlite3_step(stmt);

    const int course_id = sqlite3_column_int(stmt, 0);

    return course_id;
}

char *get_course_name_by_id(sqlite3 *db, int course_id)
{
    int rc = 0;

    const char *sql = "SELECT name FROM courses WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, course_id);
    }

    sqlite3_step(stmt);

    char *course_name = (char *)sqlite3_column_text(stmt, 0);

    return course_name;
}

void set_course_completed(APP_CONTEXT *ctx)
{
    int rc = 0;

    sqlite3_stmt *res;

    const char *sql =
        "INSERT INTO completed_courses (user_id, course_id, completed_at) "
        "VALUES (?, ?, ?);";

    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &res, 0);

    if (rc == SQLITE_OK)
    {
        char *datetime = current_datetime();
        sqlite3_bind_int(res, 1, ctx->current_user_id);
        sqlite3_bind_int(res, 2, ctx->current_course_id);
        sqlite3_bind_text(res, 3, datetime, strlen(datetime), NULL);
    }
    else
    {
        mvwprintw(ctx->course_windows[4], 1, 2, "SQL error: %s\n",
                  sqlite3_errmsg(ctx->db));
    }

    sqlite3_step(res);
}

void get_course_progress(APP_CONTEXT *ctx)
{
    int rc = 0;

    const char *sql = "SELECT section_id, items_completed FROM progress WHERE "
                      "user_id = 1 AND course_id = ?";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        return;
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_course_id);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const int section_id = sqlite3_column_int(stmt, 0);
        const int items_completed = sqlite3_column_int(stmt, 1);

        ctx->rp_state->course_progress[section_id] = items_completed;
    }

    sqlite3_finalize(stmt);
}

void get_task(APP_CONTEXT *ctx)
{
    int rc = 0;

    const char *sql =
        "SELECT task FROM tasks WHERE course_id = ? AND section_id = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_course_id);
        sqlite3_bind_int(stmt, 2, ctx->rp_state->curr_section);
    }

    sqlite3_step(stmt);

    char *task = (char *)sqlite3_column_text(stmt, 0);

    ctx->rp_state->current_task =
        (char *)malloc((strlen(task) + 1) * sizeof(char));

    ctx->rp_state->current_task = strdup(task);
}

char *get_ascii_art(sqlite3 *db, char *ascii_art)
{
    const unsigned char *ascii = malloc(2048);

    int rc = 0;

    const char *sql = "SELECT content FROM ascii_art WHERE name = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, ascii_art, strlen(ascii_art), NULL);
    }

    sqlite3_step(stmt);

    ascii = sqlite3_column_text(stmt, 0);

    return (char *)ascii;
}

char *get_end_of_course_msg(sqlite3 *db, int course_id)
{
    const unsigned char *end_of_course_msg = malloc(2048);

    int rc = 0;

    const char *sql = "SELECT end_of_course_msg FROM courses WHERE id = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, course_id);
    }

    sqlite3_step(stmt);

    end_of_course_msg = sqlite3_column_text(stmt, 0);

    return (char *)end_of_course_msg;
}

int get_completed_courses(APP_CONTEXT *ctx)
{
    int rc = 0;

    const char *sql = "SELECT COUNT(*) FROM completed_courses WHERE user_id = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ctx->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
    }

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, ctx->current_user_id);
    }

    sqlite3_step(stmt);

    return sqlite3_column_int(stmt, 0);
}
