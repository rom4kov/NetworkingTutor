#include "../data/data_access_layer.h"
#include <menu.h>
#include <ncurses.h>

void handle_greeter_input(APP_CONTEXT *ctx);
void create_new_user_popup_form(WINDOW **inner_win, WINDOW **form_window,
                                FORM **new_file_form, FIELD **field,
                                char *label);
void create_new_user_popup(APP_CONTEXT *ctx);

void handle_start_input(APP_CONTEXT *ctx);
void handle_course_input(APP_CONTEXT *ctx);
void handle_progress_input(APP_CONTEXT *ctx);
void handle_nav_input(APP_CONTEXT *ctx);

// editor functions
void move_down(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window, WINDOW **editor_window,
               int y, int x, int *scroll_offset, int lines_to_print);
void move_up(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window, WINDOW **editor_window,
             int y, int x, int *scroll_offset, int *lines_to_print);
void move_right(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window, int y, int x);
void move_left(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window, int y, int x);
void move_to_start_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window, int y,
                           int x);
void move_to_end_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window, int y, int x);
void insert_char(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                 WINDOW **editor_window, int scroll_offset, int y, int x,
                 char ch, WINDOW **line_num_win, int *lines_to_print);
void insert_tab(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
                int y, int x, WINDOW **line_num_win, int *lines_to_print,
                int *scroll_offset);
void bs_delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
                    WINDOW **line_num_win, int y, int *scroll_offset,
                    int *lines_to_print);
void delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         WINDOW **edit_window, WINDOW **editor_window, int y, int x, int *scroll_offset,
                         int *lines_to_print);
void bs_delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                            WINDOW **edit_window, WINDOW **editor_window, int y, int x,
                            int *scroll_offset, int *lines_to_print);
void insert_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window, WINDOW **line_num_win,
                 int y, int x, int *scroll_offset, int *lines_to_print);

// course instructions functions
void complete_section(APP_CONTEXT *ctx);
void complete_course(APP_CONTEXT *ctx);

char *wrap_text(char *text, int width);
void draw_border(WINDOW *win, int color_pair, char *label);
void draw_progress_border(WINDOW *win, int color_pair, int screen);
void focus_window(WINDOW **window, int color_pair, char *label);
void focus_instructions_window(RIGHT_PANEL_STATE *rps, int color_pair, char *label);
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
                int lines_to_print, int y, int x);
void log_course_instr_values(APP_CONTEXT *ctx);

// terminal
void create_pseudo_terminal(APP_CONTEXT *ctx);
void read_term_input_and_write_to_pty(APP_CONTEXT *ctx);
void read_term_output_and_print_in_term(APP_CONTEXT *ctx);
