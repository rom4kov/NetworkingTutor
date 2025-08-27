#include "../models/models.h"
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
// void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
//                          TEXT_BUFFER *text_buf, FILE *file, bool *editor_mode,
//                          int *scroll_offset, int *lines_to_print, int *y, int *x);
void handle_editor_input(APP_CONTEXT *ctx);
void handle_explorer_input(APP_CONTEXT *ctx);
void handle_terminal_input(APP_CONTEXT *ctx);
void disable_echo(int fd);
void append_term_ouput_to_buf(char buf[], int buf_len, TEXT_BUFFER *term_buf);
void print_term_buf(WINDOW *term_win, TEXT_BUFFER *term_buf);
