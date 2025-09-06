#include "../models/models.h"
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

// start view controller functions
void go_to_course_by_id(APP_CONTEXT *ctx, int course_id);

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
// void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
//                          TEXT_BUFFER *text_buf, FILE *file, bool *editor_mode,
//                          int *scroll_offset, int *lines_to_print, int *y, int *x);
void handle_editor_input(APP_CONTEXT *ctx);
void handle_explorer_input(APP_CONTEXT *ctx);
void handle_terminal_input(APP_CONTEXT *ctx);
void disable_echo(int fd);
void print_term_buf(WINDOW *term_win, TEXT_BUFFER *term_buf);

void handle_start_opts_menu_input(APP_CONTEXT *ctx, MENU *start_options_menu,
                                  int num_of_users);
void handle_new_user_input(APP_CONTEXT *ctx, bool *start_opt_menu_active);
void handle_user_select_win_input(APP_CONTEXT *ctx, bool *start_opt_menu_active,
                                  bool continue_course);

