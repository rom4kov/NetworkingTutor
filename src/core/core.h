#include "../data/data_access_layer.h"
#include <menu.h>
#include <ncurses.h>

void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                   COURSE courses[], sqlite3 **db);
void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu,
                         sqlite3 *db);
void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         int y, int x, CHAR_BUFFER *char_buf,
                         LINE_BUFFER *line_buf, int *gap_size,
                         bool *editor_mode, FILE *file, int file_size,
                         int *new_file_size);
void write_buffer_to_file(CHAR_BUFFER *cbuf, FILE *file, int file_size,
                          int *new_file_size, WINDOW *edit_window, char *ccur,
                          char *cend, LINE_BUFFER *lbuf);
void update_edit_window(CHAR_BUFFER *cbuf, LINE_BUFFER *lbuf,
                        int *new_file_size, int *gap_size, WINDOW *line_num_win,
                        WINDOW *edit_window);
int get_line_length(LINE_BUFFER *lbuf, int line_index);
void update_lines(LINE_BUFFER *lbuf, int mode);
char *wrap_text(char *text, int width);
void draw_border(WINDOW *win, int color_pair, int active_window);
void focus_window(WINDOW **windows, int window, int color_pair,
                  int *active_window, char *label);
void trim(char *str);
int get_length(char *str);
