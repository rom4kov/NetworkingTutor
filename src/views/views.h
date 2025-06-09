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

void create_start_screen(APP_CONTEXT *ctx);
WINDOW *create_navigation_window(int *active_win, MENU **start_menu);
WINDOW *create_header_section(int *active_win);
WINDOW *create_course_preview_card(int x_position, int *active_win,
                                   int curr_win_idx, COURSE *course);
WINDOW *create_right_side_panel(APP_CONTEXT *ctx, char *label);
void print_course_instructions(APP_CONTEXT *ctx, int course_num);
void init_right_panel_state(RIGHT_PANEL_STATE *rp_state,
                            bool course_view_active);
void print_next_course_item(int item, RIGHT_PANEL_STATE *rp_state);

void print_intro(WINDOW **right_panel, int window_width, int intro_width);

void create_course_view(APP_CONTEXT *ctx);
WINDOW *create_editor_window(int *active_window);
WINDOW *create_explorer_window(FILE_TREE *file_tree);
void create_explorer_menu(WINDOW **explorer_window, FILE_TREE *f_tree);
WINDOW *create_progress_window();
void print_no_open_file_msg(WINDOW **editor_window);

void create_user_form(WINDOW **window, FORM **user_form, FIELD **fields);

void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         int *scroll_offset, int lines_to_print);
void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win, int *scroll_offset,
                  int lines_to_print);
void print_line(char *line_buf, int line_num, WINDOW **edit_window);
char *match_file_icon(pcre2_code *re, int subj_len, char *filename, char *icon,
                    int color);
ICON get_file_icon(char *filename);

void print_entries(FILE_TREE *f_tree, WINDOW **explorer_window);
void move_to_next_entry(FILE_TREE *f_tree, WINDOW **explorer_window);
void move_to_prev_entry(FILE_TREE *f_tree, WINDOW **explorer_window);
void print_no_open_file_msg(WINDOW **editor_window);
