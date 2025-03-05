#include <sqlite3.h>
#include "ncurses.h"
#include "../models/models.h"

sqlite3 *create_database(WINDOW *win);
USER_DATA get_user_data(sqlite3 *db);
void seed_courses_data(sqlite3 *db, WINDOW *win);
COURSE *get_course_data(sqlite3 *db);
int callback(void *NotUsed, int argc, char **argv, char **azColName);
void update_user(sqlite3 *db, int id, char *name, char *language);

TEXT_BUFFER *initialize_buffer();
LINE *initialize_line();
void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
