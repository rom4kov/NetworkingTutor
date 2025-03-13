#include "../models/models.h"
#include <stdio.h>
#include <ncurses.h>
#include <menu.h>

TEXT_BUFFER *initialize_buffer();
LINE *initialize_line();
void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         TEXT_BUFFER *text_buf, FILE *file, bool *editor_mode);
void handle_explorer_input(int ch, WINDOW **explorer_win, bool *explorer_mode,
                           MENU **explorer_menu);

