#include "../models/models.h"
#include <stdio.h>
#include <ncurses.h>
#include <menu.h>

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         TEXT_BUFFER *text_buf, FILE *file, bool *editor_mode);
void handle_explorer_input(int ch, TEXT_BUFFER *tbuf, FILE *file,
                           WINDOW **explorer_win, WINDOW **line_num_win,
                           WINDOW **editor_window, WINDOW **edit_window,
                           bool *explorer_mode, MENU **explorer_menu);

