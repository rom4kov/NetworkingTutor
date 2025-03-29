#include "../models/models.h"
#include "form.h"
#include "menu.h"
#include "ncurses.h"
#include <sqlite3.h>

sqlite3 *create_database();
USER_DATA get_user_data(sqlite3 *db);
void seed_courses_data(sqlite3 *db, WINDOW *win);
COURSE *get_course_data(sqlite3 *db);
int callback(void *NotUsed, int argc, char **argv, char **azColName);
void update_user(sqlite3 *db, int id, char *name, char *language);

DIR_ENTRY *initialize_dir_entry();
FILE_TREE *initialize_file_tree();
TEXT_BUFFER *initialize_buffer();
LINE *initialize_line();
void prepare_empty_file(TEXT_BUFFER **tbuf);
void open_new_file(APP_CONTEXT *ctx);
void open_file(APP_CONTEXT *ctx);
void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
void write_buffer_to_file(TEXT_BUFFER *tbuf, FILE *file, int y);
void deallocate_buffer(TEXT_BUFFER *tbuf);
void create_new_file_input(WINDOW **inner_win, WINDOW **form_window,
                           FORM **new_file_form, FIELD **field, char *label);
void open_sub_directory(char *dir_name, int *dir_size, int *items, ITEM **menu_items,
                        DIR_ENTRY *entries);
