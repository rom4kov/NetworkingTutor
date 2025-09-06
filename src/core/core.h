#include "../data/data_access_layer.h"
#include <menu.h>
#include <ncurses.h>

void handle_greeter_input(APP_CONTEXT *ctx);
void create_new_user_popup(APP_CONTEXT *ctx);

void handle_start_input(APP_CONTEXT *ctx);
void handle_course_input(APP_CONTEXT *ctx);
void handle_progress_input(APP_CONTEXT *ctx);
void handle_nav_input(APP_CONTEXT *ctx);

// editor functions
void move_down(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
               WINDOW **editor_window, int y, int x, int *scroll_offset,
               int lines_to_print, int editor_height);
void move_up(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
             WINDOW **editor_window, int y, int x, int *scroll_offset,
             int *lines_to_print, int editor_height);
void move_right(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
                int y, int x, int editor_height);
void move_left(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
               int y, int x, int editor_height);
void move_to_start_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                           WINDOW **editor_window, int y, int x,
                           int editor_height);
void move_to_end_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                         WINDOW **editor_window, int y, int x,
                         int editor_height);
void insert_char(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                 WINDOW **editor_window, int scroll_offset, int y, int x,
                 char ch, WINDOW **line_num_win, int *lines_to_print,
                 int editor_height);
void insert_tab(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
                int y, int x, WINDOW **line_num_win, int *lines_to_print,
                int *scroll_offset, int editor_height);
void bs_delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                    WINDOW **editor_window, WINDOW **line_num_win, int y,
                    int *scroll_offset, int *lines_to_print, int editor_height);
void delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         WINDOW **edit_window, WINDOW **editor_window, int y,
                         int x, int *scroll_offset, int *lines_to_print,
                         int editor_height);
void bs_delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                            WINDOW **edit_window, WINDOW **editor_window, int y,
                            int x, int *scroll_offset, int *lines_to_print,
                            int editor_height);
void insert_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                 WINDOW **editor_window, WINDOW **line_num_win, int y, int x,
                 int *scroll_offset, int *lines_to_print, int editor_height);

// course instructions functions
void complete_section(APP_CONTEXT *ctx);
void complete_course(APP_CONTEXT *ctx);

char *wrap_text(char *text, int width);
void draw_border(WINDOW *win, int color_pair, char *label);
void draw_progress_border(WINDOW *win, int color_pair, int screen);
void focus_window(WINDOW **window, int color_pair, char *label);
void focus_instructions_window(RIGHT_PANEL_STATE *rps, int color_pair,
                               char *label);

const char* get_rank_name(Rank rank);
Rank determine_rank(int points);
Rank get_next_rank(Rank r);

void trim(char **str);
char *return_trimmed(char *str);
char *trunc_str(char *str, int win_width, int offset);
int strip_ansi_escape_codes(char *buf, int len);
int c_round(float x);
char *current_datetime();
int get_diff_time_in_days(APP_CONTEXT *ctx, char time1[], char time2[]);
int suppress_stdout(void);
int restore_stdout(int saved_stdout);
void log_values(WINDOW **edit_window, int scroll_offset, TEXT_BUFFER *tbuf,
                int lines_to_print, int y, int x, int editor_height);
void log_course_instr_values(APP_CONTEXT *ctx);

// terminal
void create_pseudo_terminal(APP_CONTEXT *ctx);
void print_term_input(APP_CONTEXT *ctx);
void read_term_output_and_print_in_term(APP_CONTEXT *ctx);
void move_cursor_left(APP_CONTEXT *ctx);
void move_cursor_right(APP_CONTEXT *ctx);
void scroll_up(APP_CONTEXT *ctx);
void scroll_down(APP_CONTEXT *ctx);
void delete_char_with_back_space(APP_CONTEXT *ctx);
void submit_command(APP_CONTEXT *ctx);
void append_term_ouput_to_buf(char *buf, int buf_len,
                              TEXT_BUFFER *term_buf, char *cwd);
char *get_cwd(void);
void *popen_w_pid_ret(void *arguments);
void *check_running_proc_for_output(void *args);
void run_output_funcs(APP_CONTEXT *ctx, char buf[]);
