#pragma once

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

typedef struct line
{
  char *buf_;
  unsigned short line_num;
  unsigned short length;
  struct line *prev;
  struct line *next;
} LINE;

typedef struct text_buffer
{
  LINE *first_line;
  LINE *trav;
  LINE *current_line;
  unsigned short curr_line_nr;
  unsigned short current_col;
  unsigned short num_of_lines;
} TEXT_BUFFER;

typedef struct _char_buffer
{
    char *buf_;
    char *ccur_;
    char *cend_;
    int gap_size_;
    int size_;
    int mod_size_;
} CHAR_BUFFER;

typedef struct _line_buffer
{
    int *line_size_;
    int *new_lines_;
    int *ccur_;
    int *cend_;
    int gap_size_;
    int size_;
    int mod_size_;
} LINE_BUFFER;

typedef struct _text_editor
{
  CHAR_BUFFER cb_;
  LINE_BUFFER lb_;
} TEXT_EDITOR;
