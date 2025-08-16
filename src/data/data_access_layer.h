#include "../models/models.h"
#include "form.h"
// #include "menu.h"
#include "ncurses.h"
#include <sqlite3.h>


//////////////
// DATABASE //
//////////////

sqlite3 *create_database();
char *read_sql_query(char *filename);
int create_new_user(APP_CONTEXT *ctx, char *username);
USER_DATA *get_user_data(sqlite3 *db, int user_id);

void seed_courses_data(sqlite3 *db, WINDOW *win, char *query);
char *get_course_name_by_id(sqlite3 *db, int course_id);
COURSE *get_course_data(sqlite3 *db);
COURSE *get_course_by_id(sqlite3 *db, int course_id);

SECTION_METADATA *get_section_metadata(APP_CONTEXT *ctx);
int callback(void *NotUsed, int argc, char **argv, char **azColName);
COURSE_SECTION *get_course_section_materials(sqlite3 *db, int course,
                                       int section, int *num_of_items);
void get_total_course_sections(APP_CONTEXT *ctx);
void get_completed_sections(APP_CONTEXT *ctx);

void update_user(sqlite3 *db, int id, char *name, char *language);

int get_current_course(sqlite3 *db, int user_id);
void get_course_progress(APP_CONTEXT *ctx);
void set_items_completed(APP_CONTEXT *ctx);
int get_total_completed_items(APP_CONTEXT *ctx);
void set_section_completed(APP_CONTEXT *ctx);
void set_course_completed(APP_CONTEXT *ctx);
int get_completed_courses(APP_CONTEXT *ctx);
int get_total_completed_sections(APP_CONTEXT *ctx);

void get_task(APP_CONTEXT *ctx);
char *get_ascii_art(sqlite3 *db, char *ascii_art);
char *get_end_of_course_msg(sqlite3 *db, int course_id);


/////////////
// FILE IO //
/////////////

DIR_ENTRY *initialize_dir_entry();
FILE_TREE *initialize_file_tree();
TEXT_BUFFER *initialize_buffer();
I_TEXT_BUFFER *initialize_it_buffer();
LINE *initialize_line();
I_LINE *initialize_iline();
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
                               char *new_filename, int type);
void remove_entry_from_file_tree(FILE_TREE *f_tree);
void delete_file(APP_CONTEXT *ctx, bool *del_file_form_active,
                 WINDOW **inner_win, WINDOW **form_window, FORM **new_file_form,
                 FIELD **field);
void rename_file(APP_CONTEXT *ctx, WINDOW **inner_win, WINDOW **form_window,
                 FORM **new_file_form, FIELD **field);
void create_directory(APP_CONTEXT *ctx, WINDOW **inner_win,
                      WINDOW **form_window, FORM **new_file_form,
                      FIELD **field);
void create_keybinds_window(WINDOW **explorer_window);
void create_keybinds_window(WINDOW **explorer_window);
