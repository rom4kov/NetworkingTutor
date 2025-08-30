#pragma once

#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <stdio.h>
#include <sys/types.h>

#define START_WINDOW_COUNT 6
#define COURSE_WINDOW_COUNT 5
#define PROGRESS_WINDOW_COUNT 5
#define ALL_COURSES_WINDOW_COUNT 3
#define KEYBINDINGS_WINDOW_COUNT 3
#define SEPARATOR 333

typedef struct _user_data
{
    const unsigned char *name;
    const unsigned char *created_at;
} USER_DATA;

typedef struct _course_data
{
    int id;
    char *name;
    char *short_desc;
    char *ascii_logo;
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
    unsigned short curr_line_nr;
    unsigned short current_col;
    unsigned short num_of_lines;
    unsigned short scroll_offset;
} TEXT_BUFFER;

typedef struct instructions_text_buffer
{
    I_LINE *first_line;
    I_LINE *current_line;
    unsigned short curr_line_nr;
    unsigned short current_col;
    unsigned short num_of_lines;
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
    char state;
    bool last_in_sub_dir;
} DIR_ENTRY;

typedef struct _file_tree
{
    DIR_ENTRY *first_entry;
    DIR_ENTRY *current_entry;
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
  WINDOW *greeter_ascii_window;
  WINDOW *start_windows[START_WINDOW_COUNT];
  WINDOW *course_windows[COURSE_WINDOW_COUNT];
  WINDOW *terminal_window;
  WINDOW *progress_windows[PROGRESS_WINDOW_COUNT];
  WINDOW *all_courses_windows[ALL_COURSES_WINDOW_COUNT];
  WINDOW *keybindings_windows[ALL_COURSES_WINDOW_COUNT];
  WINDOW *line_num_win;
  WINDOW *edit_window;
  WINDOW *active_window;
  USER_DATA *user_data;
  SHELL *shell;
  RIGHT_PANEL_STATE *rp_state;
  COURSE *courses;
  MENU *greeter_menu;
  MENU *start_menu;
  MENU *explorer_menu;
  ITEM **menu_items;
  ITEM *curr_item;
  FORM *user_form;
  FIELD *user_form_fields;
  FILE_TREE *file_tree;
  FILE *file;
  char *filename;
  char *curr_file_path;
  char *current_course;
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
  int y, x;
  int scroll_offset;
  int lines_to_print;
  int curr_line;
  int curr_col;
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
  bool is_in_failure_list;
  bool user_form_active;
  bool terminal_active;
} APP_CONTEXT;

typedef struct _thread_args
{
  char *cmd;
  FILE *file;
  pid_t *pid;
  WINDOW *win;
} THREAD_ARGS;

typedef struct _output_thread_args
{
  FILE *file;
  pid_t *pid;
  APP_CONTEXT *ctx;
  char buf[BUFSIZ];
  WINDOW *win;
  unsigned short stop_flag;
} OUTPUT_THREAD_ARGS;
