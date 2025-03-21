#include "../data/data_access_layer.h"
#include <menu.h>
#include <ncurses.h>

void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                   COURSE courses[], sqlite3 **db);
void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu,
                         MENU **explorer_menu, ITEM ***menu_items, sqlite3 *db);
void move_down(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
               int y, int x, int *scroll_offset, int lines_to_print);
void move_up(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
             int y, int x, int *scroll_offset, int *lines_to_print);
void move_right(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x);
void move_left(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x);
void move_to_start_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y,
                           int x);
void move_to_end_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x);
void insert_char(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x,
                 char ch);
void insert_tab(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x);
void bs_delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                    WINDOW **line_num_win, int y, int *scoll_offset,
                    int *lines_to_print);
void delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         WINDOW **edit_window, int y, int x, int *scoll_offset,
                         int *lines_to_print);
void bs_delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                            WINDOW **edit_window, int y, int x,
                            int *scoll_offset, int *lines_to_print);
void insert_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **line_num_win,
                 int y, int x, int *scoll_offset, int *lines_to_print);

char *wrap_text(char *text, int width);
void draw_border(WINDOW *win, int color_pair, char *label);
void focus_window(WINDOW **window, int color_pair, char *label);
void trim(char *str);
int get_length(char *str);
