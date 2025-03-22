#include "../models/models.h"
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         TEXT_BUFFER *text_buf, FILE *file, bool *editor_mode,
                         int *scroll_offset, int *lines_to_print);
void handle_explorer_input(int ch, TEXT_BUFFER *tbuf, FILE **file,
                           char **filename, WINDOW **explorer_win,
                           WINDOW **line_num_win, WINDOW **editor_window,
                           WINDOW **edit_window, bool *editor_mode,
                           bool *explorer_mode, MENU **explorer_menu,
                           ITEM ***menu_items, int *scroll_offset,
                           int *lines_to_print, int *active_window);
