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

// greeter screen
void create_greeter_screen(APP_CONTEXT *ctx);
MENU *create_greeter_menu(APP_CONTEXT *ctx);
WINDOW *create_start_options_popup();
MENU *create_start_options_menu(APP_CONTEXT *ctx, WINDOW **start_opt_menu_win,
                                int num_of_users);
FORM *create_new_user_popup_form(APP_CONTEXT *ctx, char *label);
MENU *create_user_selection_menu(APP_CONTEXT *ctx, int num_of_users);
void deallocate_greeter_memory(APP_CONTEXT *ctx);

// start screen
void create_start_screen(APP_CONTEXT *ctx);
WINDOW *create_navigation_window(APP_CONTEXT *ctx);
WINDOW *create_header_section(APP_CONTEXT *ctx);
WINDOW *create_course_preview_card(APP_CONTEXT *ctx, int x_position,
                                   int curr_win_idx, COURSE *course);
WINDOW *create_right_side_panel(APP_CONTEXT *ctx, char *label);
void cleanup_start_for_switch(APP_CONTEXT *ctx);
void cleanup_start_for_exit(APP_CONTEXT *ctx);
// void read_item_into_buffer(COURSE_SECTION *c_sec, TEXT_BUFFER *text_buf);


// course view
void create_course_view(APP_CONTEXT *ctx);
WINDOW *create_editor_window(APP_CONTEXT *ctx);
WINDOW *create_terminal_window(APP_CONTEXT *ctx);
WINDOW *create_explorer_window(FILE_TREE *file_tree);
void create_explorer_menu(WINDOW **explorer_window, FILE_TREE *f_tree);
WINDOW *create_progress_window(APP_CONTEXT *ctx);
void print_no_open_file_msg(APP_CONTEXT *ctx);
void log_course_instr_values(APP_CONTEXT *ctx);
void recreate_editor_windows(APP_CONTEXT *ctx);

void add_line_break(COURSE_SECTION *c_sec_data, I_TEXT_BUFFER *tbuf,
                    I_LINE **curr_line, int i, int *j, int *k, int *line_number,
                    int *last_space_pos, bool overflow, bool *bl_point);
void read_items_into_buffer(APP_CONTEXT *ctx);
void read_task_into_buffer(APP_CONTEXT *ctx);
void read_end_of_course_page_into_buffer(APP_CONTEXT *ctx);
void read_window_text_into_buffer(APP_CONTEXT *ctx, I_TEXT_BUFFER *header_tbuf,
                                  int win_width, int win, int course_id,
                                  char *add_text);
void read_in_buf_str(char *buf_str, int total, I_TEXT_BUFFER *header_tbuf,
                     int win_width, int win);

void print_course_instructions(APP_CONTEXT *ctx);
void init_right_panel_state(RIGHT_PANEL_STATE *rp_state,
                            bool course_view_active);
void print_next_course_item(RIGHT_PANEL_STATE *rp_state);
void print_window_content(I_TEXT_BUFFER *header_tbuf, WINDOW *win,
                          int win_width);

void print_intro(APP_CONTEXT *ctx);
void print_press_msg(RIGHT_PANEL_STATE *rps);

void deallocate_course_view_memory(APP_CONTEXT *ctx);
void free_section_data(APP_CONTEXT *ctx);
void cleanup_nav_menu(APP_CONTEXT *ctx);

void create_user_form(WINDOW *window, FORM **user_form, FIELD **fields);

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
void print_line_nr(WINDOW **editor_window, TEXT_BUFFER *tbuf,
                   int editor_height);
char *match_file_icon(pcre2_code *re, int subj_len, char *filename, char *icon,
                      int color);
ICON get_file_icon(char *filename);

void print_entries(FILE_TREE *f_tree, WINDOW **explorer_window);
void move_to_next_entry(FILE_TREE *f_tree, WINDOW **explorer_window);
void move_to_prev_entry(FILE_TREE *f_tree, WINDOW **explorer_window);
void print_course_complete(APP_CONTEXT *ctx);

void print_section_or_task_compl_msg(APP_CONTEXT *ctx, CU_pRunSummary rs);

// progress view
void create_progress_view(APP_CONTEXT *ctx);
WINDOW *create_account_window(APP_CONTEXT *ctx);
WINDOW *create_progress_screen(APP_CONTEXT *ctx);
void create_your_courses_window(APP_CONTEXT *ctx, WINDOW *win);
WINDOW *create_progress_stats_window(APP_CONTEXT *ctx);
void print_completed_courses(APP_CONTEXT *ctx);
void print_your_courses_title(APP_CONTEXT *ctx);
void print_no_courses_started_msg(WINDOW *win, int height, int width);

// all courses view
void create_all_courses_view(APP_CONTEXT *ctx);
WINDOW *create_all_courses_screen();
WINDOW *create_all_courses_container(APP_CONTEXT *ctx);
void print_all_courses(APP_CONTEXT *ctx);

// keybindings view
void create_keybindings_view(APP_CONTEXT *ctx);
WINDOW *create_keybindings_screen(void);
WINDOW *create_keybindings_container(APP_CONTEXT *ctx);
