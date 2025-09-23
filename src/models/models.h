#pragma once

#include "../../ntutor.h"
#include "stdatomic.h"
#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <sqlite3.h>
#include <stdio.h>
#include <sys/types.h>

typedef struct _user_data
{
    char *name;
    char *created_at;
    char *home_dir;
} USER_DATA;

typedef struct _course_data
{
    int id;
    char *name;
    char *short_desc;
    char *ascii_logo;
    bool locked;
} COURSE;

typedef struct _course_section
{
    int id;
    int course_id;
    int section_id;
    char *content_title;
    char *content;
    int order_num;
    bool syntax_hl;
} COURSE_SECTION;

typedef struct _section_metadata
{
    char *title;
    bool has_test;
    bool has_separate_task;
} SECTION_METADATA;

typedef struct _progress_data
{
    int id;
    int user_id;
    int course_id;
    int *course_progress;
} PROGRESS_DATA;

typedef struct _line
{
    struct _line *prev;
    struct _line *next;
    unsigned short line_num;
    unsigned short length;
    char *buf_;
} LINE;

typedef struct _instructions_line
{
    struct _instructions_line *prev;
    struct _instructions_line *next;
    unsigned short line_num;
    unsigned short length;
    char *buf_;
    int style;
    bool centered;
    bool syntax_hl;
} I_LINE;

typedef struct text_buffer
{
    LINE *first_line;
    LINE *current_line;
    int curr_line_nr;
    int current_col;
    int num_of_lines;
    int scroll_offset;
    int lines_to_print;
} TEXT_BUFFER;

typedef struct instructions_text_buffer
{
    I_LINE *first_line;
    I_LINE *current_line;
    int curr_line_nr;
    int current_col;
    int num_of_lines;
} I_TEXT_BUFFER;

typedef struct _icon
{
    int color;
    char *icon;
} ICON;

typedef struct _dir_entry
{
    struct _dir_entry *prev;
    struct _dir_entry *next;
    struct _dir_entry *parent_dir;
    char *name;
    char *path;
    int type;
    int indent_level;
    int num_of_entries;
    int num_of_open_entries;
    char state;
    bool last_in_sub_dir;
} DIR_ENTRY;

typedef struct _file_tree
{
    DIR_ENTRY *first_entry;
    DIR_ENTRY *current_entry;
    DIR_ENTRY *prev_dir;
    int curr_entry_nr;
    int num_of_entries;
} FILE_TREE;

typedef struct _pseudo_terminal
{
    TEXT_BUFFER *term_buffer;
    WINDOW *term_inner_win;
    char buf[BUFSIZ];
    char *cwd;
    char *home_dir;
    int curr_buf_idx;
    bool terminal_active;
    bool terminal_focused;
    bool cwd_allocated;
    atomic_bool executable_running;
    atomic_bool stop_executable;
    pid_t child_pid;
    FILE *child_stream;
    int child_fd;
    pthread_t reader_thread;
} SHELL;

typedef struct _right_panel_state
{
    I_TEXT_BUFFER *it_buffer;
    COURSE_SECTION *course_section_data;
    SECTION_METADATA *s_metadata;
    WINDOW *right_panel;
    WINDOW *header_win;
    WINDOW *inner_win;
    char *current_task;
    bool *completed_sections;
    bool ready_to_test;
    bool showing_test_results;
    bool showing_end_of_course_page;
    int *course_progress;
    int num_of_section_items[32];
    int *total_section_items;
    int total_course_sections;
    int window_width;
    int intro_width;
    int curr_section;
    int curr_item;
    int scroll_offset;
    int sections_completed;
    int items_completed;
    int items_to_print;
    int curr_offset;
    int lines_to_print;
    int lines_excess;
    int y, x;
} RIGHT_PANEL_STATE;

typedef struct _app_context
{
    sqlite3 *db;
    WINDOW *greeter_screen;
    WINDOW *greeter_windows[GREETER_WINDOW_COUNT];
    WINDOW *greeter_ascii_window;
    WINDOW *start_windows[START_WINDOW_COUNT];
    WINDOW *course_windows[COURSE_WINDOW_COUNT];
    WINDOW *explorer_popup_windows[EXPLORER_POPUP_WINDOW_COUNT];
    WINDOW *terminal_window;
    WINDOW *progress_windows[PROGRESS_WINDOW_COUNT];
    WINDOW *all_courses_windows[ALL_COURSES_WINDOW_COUNT];
    WINDOW *keybindings_windows[ALL_COURSES_WINDOW_COUNT];
    WINDOW *line_num_win;
    WINDOW *edit_window;
    WINDOW *active_window;
    PANEL *greeter_panels[GREETER_PANEL_COUNT];
    PANEL *explorer_panels[EXPLORER_PANEL_COUNT];
    USER_DATA *user_data;
    SHELL *shell;
    RIGHT_PANEL_STATE *rp_state;
    COURSE *courses;
    MENU *greeter_menu;
    MENU *greeter_start_opts_menu;
    MENU *greeter_user_select_menu;
    MENU *start_menu;
    MENU *explorer_menu;
    ITEM **greeter_menu_items;
    ITEM **greeter_start_opts_menu_items;
    ITEM **greeter_user_select_menu_items;
    ITEM **nav_menu_items;
    ITEM *curr_item;
    FORM *new_user_form;
    FORM *user_form;
    FIELD *user_form_field[2];
    FIELD *new_user_form_field[2];
    FILE_TREE *file_tree;
    FILE *file;
    char *user_home_dir;
    char *home_env;
    char *shell_local_cwd;
    char *filename;
    char *curr_file_path;
    char *current_course;
    char **user_select_menu_strings;
    TEXT_BUFFER *t_buffer;
    I_TEXT_BUFFER *intro_buffer;
    I_TEXT_BUFFER *card_buffers[10];
    CU_ErrorCode ec;
    CU_pSuite sp[15];
    CU_pRunSummary run_sum;
    int key;
    int active_window_idx;
    int current_user_id;
    int current_course_id;
    int num_of_users;
    int num_of_courses;
    int y, x;
    int editor_height;
    int scroll_offset;
    int lines_to_print;
    int curr_line;
    int curr_col;
    int curr_nav_item;
    int filename_len;
    int file_size;
    bool running;
    bool greeter_needs_redraw;
    bool start_needs_redraw;
    bool course_needs_redraw;
    bool progress_needs_redraw;
    bool all_courses_needs_redraw;
    bool keybindings_needs_redraw;
    bool greeter_view_active;
    bool start_view_active;
    bool course_view_active;
    bool progress_view_active;
    bool all_courses_view_active;
    bool keybindings_view_active;
    bool first_greeter_draw;
    bool first_start_draw;
    bool first_course_draw;
    bool first_progress_draw;
    bool first_all_courses_draw;
    bool first_keybindings_draw;
    bool editor_mode;
    bool explorer_mode;
    bool file_modified;
    bool is_in_failure_list;
    bool user_form_active;
    bool terminal_active;
} APP_CONTEXT;

typedef struct _thread_args
{
    APP_CONTEXT *ctx;
    WINDOW *win;
    FILE *file;
    char *cmd;
    pid_t *pid;
} THREAD_ARGS;

typedef struct _output_thread_args
{
    FILE *file;
    pid_t *pid;
    APP_CONTEXT *ctx;
    char buf[BUFSIZ];
    WINDOW *win;
} OUTPUT_THREAD_ARGS;

typedef enum
{
    RANK_PACKET_NOVICE = 0,
    RANK_LAYER_SHIKOMI = 18,
    RANK_ADDRESSINFO_DESHI = 39,
    RANK_SOCKET_APPRENTICE = 68,
    RANK_HTTP_BUSHI = 270,
    RANK_PARSER_NINJA = 400,
    RANK_ROUTER_RONIN = 600,
    RANK_PROTOCOL_KENSEI = 800,
    RANK_ENCRYPTION_SHOGUN = 1000,
    RANK_MULTITHREAD_DAIMYO = 1300,
    RANK_STREAM_SENSEI = 1600,
    RANK_NETWORK_TENGU = 2000
} Rank;
