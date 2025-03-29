#pragma once

#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>

#define START_WINDOW_COUNT 6
#define COURSE_WINDOW_COUNT 4

typedef struct _user_data
{
    const unsigned char *name;
    const unsigned char *language;
} USER_DATA;

typedef struct _course_data
{
    int id;
    char *name;
    char *short_desc;
} COURSE;

typedef struct _line
{
    struct _line *prev;
    struct _line *next;
    unsigned short line_num;
    unsigned short length;
    char *buf_;
} LINE;

typedef struct text_buffer
{
    LINE *first_line;
    LINE *current_line;
    unsigned short curr_line_nr;
    unsigned short current_col;
    unsigned short num_of_lines;
} TEXT_BUFFER;

typedef struct _icon
{
    int color;
    char *icon;
} ICON;

typedef struct _dir_entry
{
    struct _dir_entry *prev;
    struct _dir_entry *next;
    int type;
    char *name;
    char state;
} DIR_ENTRY;

typedef struct _file_tree
{
    DIR_ENTRY *first_entry;
    DIR_ENTRY *current_entry;
    unsigned short curr_entry_nr;
    unsigned short num_of_entries;
} FILE_TREE;

typedef struct _app_context
{
    sqlite3 *db;
    WINDOW *start_windows[START_WINDOW_COUNT];
    WINDOW *course_windows[COURSE_WINDOW_COUNT];
    WINDOW *line_num_win;
    WINDOW *edit_window;
    COURSE *courses;
    MENU *start_menu;
    MENU *explorer_menu;
    ITEM **menu_items;
    ITEM *curr_item;
    FILE_TREE *file_tree;
    FILE *file;
    char *filename;
    TEXT_BUFFER *t_buffer;
    int key;
    int active_window;
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
    bool start_view_active;
    bool course_view_active;
    bool editor_mode;
    bool explorer_mode;
} APP_CONTEXT;
