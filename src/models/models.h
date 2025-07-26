#pragma once

#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <CUnit/Basic.h>
#include <CUnit/CUError.h>

#define START_WINDOW_COUNT 6
#define COURSE_WINDOW_COUNT 5
#define SEPARATOR 333
// #define BOLD 1
// #define UNDERLINE 2
// #define CENTERED 4
// #define O_LIST 8
// #define U_LIST 16

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
    bool test_mode;
    int *course_progress;
    int num_of_section_items[32];
    int *total_section_items;
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
    WINDOW *start_windows[START_WINDOW_COUNT];
    WINDOW *course_windows[COURSE_WINDOW_COUNT];
    WINDOW *line_num_win;
    WINDOW *edit_window;
    RIGHT_PANEL_STATE *rp_state;
    COURSE *courses;
    MENU *greeter_menu;
    MENU *start_menu;
    MENU *explorer_menu;
    ITEM **menu_items;
    ITEM *curr_item;
    FILE_TREE *file_tree;
    FILE *file;
    char *filename;
    char *curr_file_path;
    char *current_course;
    TEXT_BUFFER *t_buffer;
    CU_ErrorCode ec; 
    CU_pSuite sp[15];
    int key;
    int active_window;
    int current_user_id;
    int current_course_id;
    int y, x;
    int scroll_offset;
    int lines_to_print;
    int curr_line;
    int curr_col;
    bool running;
    bool start_needs_redraw;
    bool first_start_draw;
    bool first_course_draw;
    bool course_needs_redraw;
    bool greeter_view_active;
    bool start_view_active;
    bool course_view_active;
    bool editor_mode;
    bool explorer_mode;
    bool is_in_failure_list;
} APP_CONTEXT;
