#include "../data/data_access_layer.h"
#include "form.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>

void initialize_colors();
WINDOW *create_welcome_screen();
void create_start_screen(sqlite3 *db);
WINDOW *create_navigation_window(int *active_win, MENU **start_menu);
WINDOW *create_header_section(int *active_win);
WINDOW *create_course_preview_card(int x_position, int *active_win,
                                   int curr_win_idx, COURSE *course);
WINDOW *create_right_side_panel(int *active_win, sqlite3 **db, char *label);
void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                   COURSE courses[], sqlite3 **db);

void create_course_view(sqlite3 *db);
WINDOW *create_editor_window(int *active_window);
WINDOW *create_explorer_window(int *active_window, MENU **explorer_menu);
void create_explorer_menu(WINDOW **explorer_window, MENU **explorer_menu);
void create_user_form(WINDOW **window, FORM **user_form, FIELD **fields);
void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win);
void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win, int *scroll_offset);
void print_line(char *line_buf, int line_num, WINDOW **edit_window);
