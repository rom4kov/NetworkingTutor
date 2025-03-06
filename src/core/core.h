#include "../data/data_access_layer.h"
#include <menu.h>
#include <ncurses.h>

void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                   COURSE courses[], sqlite3 **db);
void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu,
                         sqlite3 *db);
void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         TEXT_BUFFER *text_buf, FILE *file, bool *editor_mode);
// void write_buffer_to_file(CHAR_BUFFER *cbuf, FILE *file, int file_size,
//                           int *new_file_size, WINDOW *edit_window, char
//                           *ccur, char *cend, LINE_BUFFER *lbuf);
// void update_edit_window(CHAR_BUFFER *cbuf, LINE_BUFFER *lbuf,
//                         int *new_file_size, WINDOW *line_num_win,
//                         WINDOW *edit_window);
//
void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win);
void move_down(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
               int y, int x);
void move_up(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
             int y, int x);
void move_right(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x);
void move_left(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x);
void move_to_start_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y,
                           int x);
void move_to_end_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x);
void insert_char(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x,
                 char ch);
void bs_delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                    WINDOW **line_num_win, int y);
void delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         WINDOW **edit_window, int y, int x);
void bs_delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                            WINDOW **edit_window, int y, int x);
void insert_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **line_num_win,
                 int y, int x);

char *wrap_text(char *text, int width);
void draw_border(WINDOW *win, int color_pair, int active_window);
void focus_window(WINDOW **windows, int window, int color_pair,
                  int *active_window, char *label);
void trim(char *str);
int get_length(char *str);
