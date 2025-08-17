#include "../data/data_access_layer.h"
#include "form.h"

#include <pcre.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <CUnit/TestRun.h>
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <pcre2.h>
#include <sqlite3.h>

void initialize_colors();
WINDOW *create_greeter_screen(APP_CONTEXT *ctx);

void create_start_screen(APP_CONTEXT *ctx);
WINDOW *create_navigation_window(int *active_win, MENU **start_menu);
WINDOW *create_header_section(APP_CONTEXT *ctx);
WINDOW *create_course_preview_card(int x_position, int *active_win,
                                   int curr_win_idx, COURSE *course);
WINDOW *create_right_side_panel(APP_CONTEXT *ctx, char *label);
// void read_item_into_buffer(COURSE_SECTION *c_sec, TEXT_BUFFER *text_buf);
void add_line_break(COURSE_SECTION *c_sec_data, I_TEXT_BUFFER *tbuf,
                    I_LINE **curr_line, int i, int *j, int *k, int *line_number,
                    int *last_space_pos, bool overflow, bool *bl_point);
void read_item_into_buffer(APP_CONTEXT *ctx);
void read_task_into_buffer(APP_CONTEXT *ctx);
void read_end_of_course_page_into_buffer(APP_CONTEXT *ctx);
void read_logo_and_header_text_into_buffer(APP_CONTEXT *ctx,
                                           I_TEXT_BUFFER *header_tbuf,
                                           int win_width);

void print_course_instructions(APP_CONTEXT *ctx);
void init_right_panel_state(RIGHT_PANEL_STATE *rp_state,
                            bool course_view_active);
void print_next_course_item(RIGHT_PANEL_STATE *rp_state);
void print_logo_and_welcome_text(I_TEXT_BUFFER *header_tbuf, WINDOW *win,
                                 int win_width);

void print_intro(WINDOW **right_panel, int window_width, int intro_width);
void print_press_msg(RIGHT_PANEL_STATE *rps);
void print_course_complete(APP_CONTEXT *ctx);

void create_course_view(APP_CONTEXT *ctx);
WINDOW *create_editor_window();
WINDOW *create_explorer_window(FILE_TREE *file_tree);
void create_explorer_menu(WINDOW **explorer_window, FILE_TREE *f_tree);
WINDOW *create_progress_window(APP_CONTEXT *ctx);
void print_no_open_file_msg(WINDOW **editor_window);
void log_course_instr_values(APP_CONTEXT *ctx);

void create_user_form(WINDOW **window, FORM **user_form, FIELD **fields);

void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         int *scroll_offset, int lines_to_print);
void compile_patterns(pcre2_code **re, int p_codes_num, char **pattern_str);
void print_matches(pcre2_code **re, int line_num, int j, size_t subject_length,
                   LINE *current_line, int group, WINDOW **edit_window,
                   int color);
void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win, int *scroll_offset,
                  int lines_to_print);
void print_line(LINE *current_line, int line_num, WINDOW **edit_window);
void print_line_nr(WINDOW **edit_window, TEXT_BUFFER *tbuf);
char *match_file_icon(pcre2_code *re, int subj_len, char *filename, char *icon,
                      int color);
ICON get_file_icon(char *filename);

void print_entries(FILE_TREE *f_tree, WINDOW **explorer_window);
void move_to_next_entry(FILE_TREE *f_tree, WINDOW **explorer_window);
void move_to_prev_entry(FILE_TREE *f_tree, WINDOW **explorer_window);
void print_no_open_file_msg(WINDOW **editor_window);
void deallocate_it_buffer(I_TEXT_BUFFER *tbuf);

void print_section_or_task_compl_msg(APP_CONTEXT *ctx, CU_pRunSummary rs);

// progress view
void create_progress_view(APP_CONTEXT *ctx);
WINDOW *create_progress_screen(APP_CONTEXT *ctx);
void print_progress_stats(APP_CONTEXT *ctx);
