#include "../models/models.h"
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

// greeter sub menu input handlers
void handle_start_opts_menu_input(APP_CONTEXT *ctx, MENU *start_options_menu);
void handle_new_user_input(APP_CONTEXT *ctx, bool *start_opt_menu_active);
void handle_user_select_win_input(APP_CONTEXT *ctx, bool *start_opt_menu_active,
                                  int menu_option);
// start view controller functions
void go_to_course_by_id(APP_CONTEXT *ctx, int course_id);

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);

// course view input controllers
void handle_explorer_input(APP_CONTEXT *ctx);
void handle_inactive_editor_input(APP_CONTEXT *ctx);
void handle_editor_input(APP_CONTEXT *ctx);
void handle_terminal_input(APP_CONTEXT *ctx);
void handle_instr_window_input(APP_CONTEXT *ctx);

void disable_echo(int fd);
