#include "../data/data_access_layer.h"
#include "form.h"

#include <pcre.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <pcre2.h>
#include <sqlite3.h>

void initialize_colors();
WINDOW *create_welcome_screen();
void create_start_screen(WINDOW **windows, int *active_window,
                         MENU **start_menu, COURSE courses[], sqlite3 *db);
WINDOW *create_navigation_window(int *active_win, MENU **start_menu);
WINDOW *create_header_section(int *active_win);
WINDOW *create_course_preview_card(int x_position, int *active_win,
                                   int curr_win_idx, COURSE *course);
WINDOW *create_right_side_panel(int *active_win, sqlite3 **db, char *label);
// void handle_start_input(WINDOW **windows, int *active_win, MENU **start_menu,
//                    COURSE courses[], sqlite3 **db);

void create_course_view(WINDOW **windows, WINDOW **line_num_win,
                        WINDOW **edit_window, int *active_window,
                        MENU **start_menu, MENU **explorer_menu,
                        ITEM ***menu_items, sqlite3 *db);
WINDOW *create_editor_window(int *active_window);
WINDOW *create_explorer_window(MENU **explorer_menu, ITEM ***menu_items);
void create_explorer_menu(WINDOW **explorer_window, MENU **explorer_menu,
                          ITEM ***menu_items);
void create_user_form(WINDOW **window, FORM **user_form, FIELD **fields);
void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         int *scroll_offset, int lines_to_print);
void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win, int *scroll_offset,
                  int lines_to_print);
void print_line(char *line_buf, int line_num, WINDOW **edit_window);
char *get_file_icon(pcre2_code *re, int subj_len, char *filename, char *icon,
                    int color);
ICON print_file_icon(char *filename);
