#include "../models/models.h"
#include "form.h"
// #include "menu.h"
#include "ncurses.h"
#include <sqlite3.h>

sqlite3 *create_database();
char *read_sql_query(char *filename);
USER_DATA get_user_data(sqlite3 *db);
void seed_courses_data(sqlite3 *db, WINDOW *win);
COURSE *get_course_data(sqlite3 *db);
int callback(void *NotUsed, int argc, char **argv, char **azColName);
COURSE_SECTION *get_course_section_data(sqlite3 *db, int active_course,
                                       int section, int *num_of_items);
void update_user(sqlite3 *db, int id, char *name, char *language);

DIR_ENTRY *initialize_dir_entry();
FILE_TREE *initialize_file_tree();
TEXT_BUFFER *initialize_buffer();
LINE *initialize_line();
void prepare_empty_file(TEXT_BUFFER **tbuf);
void print_buffer_label(APP_CONTEXT *ctx);
void open_new_file(APP_CONTEXT *ctx);
void open_file(APP_CONTEXT *ctx);
void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
void write_buffer_to_file(TEXT_BUFFER *tbuf, FILE *file, int y);
void deallocate_buffer(TEXT_BUFFER *tbuf);
void create_new_file_input(WINDOW **inner_win, WINDOW **form_window,
                           FORM **new_file_form, FIELD **field, char *label);
void open_sub_directory(char *dir_name, FILE_TREE *f_tree);
void close_sub_directory(DIR_ENTRY *dir_to_close, int entries_in_dir,
                         FILE_TREE *f_tree);
void open_or_close_dir(FILE_TREE *f_tree, WINDOW **explorer_window);
void open_file_from_explorer(APP_CONTEXT *ctx, bool *new_file_form_active);
void create_new_file(APP_CONTEXT *ctx, WINDOW **form_window, WINDOW **inner_win,
                     bool *new_file_form_active, FORM **new_file_form,
                     FIELD **field);
void create_new_entry_for_file(APP_CONTEXT *ctx, DIR_ENTRY *current_entry,
                               DIR_ENTRY *next_entry, char *new_filename,
                               int type);
void remove_entry_from_file_tree(FILE_TREE *f_tree, WINDOW **win);
void delete_file(APP_CONTEXT *ctx, bool *del_file_form_active,
                 WINDOW **inner_win, WINDOW **form_window, FORM **new_file_form,
                 FIELD **field);
void rename_file(APP_CONTEXT *ctx, WINDOW **inner_win, WINDOW **form_window,
                 FORM **new_file_form, FIELD **field);
void create_directory(APP_CONTEXT *ctx, WINDOW **inner_win,
                      WINDOW **form_window, FORM **new_file_form,
                      FIELD **field);
void create_keybinds_window(WINDOW **explorer_window);
