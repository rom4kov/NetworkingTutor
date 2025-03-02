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
