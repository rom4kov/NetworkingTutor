#include "../data/data_access_layer.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>

void create_start_screen(sqlite3 *db);
WINDOW *create_navigation_window(int *active_win, MENU **start_menu);
WINDOW *create_header_section(int *active_win);
WINDOW *create_course_preview_card(int x_position, int *active_win,
                                   int curr_win_idx, COURSE *course);
WINDOW *create_right_side_panel(int *active_win, sqlite3 **db);
void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                   COURSE courses[], sqlite3 **db);
void draw_border(WINDOW *win, int color_pair, int active_window);
void focus_window(WINDOW **windows, int window, int color_pair,
                  int *active_window, char *label);
void trim(char *str);
int get_length(char *str);

void create_course_view(sqlite3 *db);
WINDOW *create_editor_window(int *active_window);
