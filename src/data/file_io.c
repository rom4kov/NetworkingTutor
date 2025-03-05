#include "../models/models.h"
#include <stdio.h>
#include <stdlib.h>

TEXT_BUFFER *initialize_buffer()
{
    TEXT_BUFFER *text_buf = malloc(sizeof(TEXT_BUFFER));

    text_buf->first_line = NULL;
    text_buf->num_of_lines = 0;
    text_buf->curr_line_nr = 0;
    text_buf->current_col = 0;

    return text_buf;
}

LINE *initialize_line()
{
    LINE *line = malloc(sizeof(LINE));

    line->buf_ = calloc(80, sizeof(char));
    line->line_num = 0;
    line->length = 1;
    line->prev = NULL;
    line->next = NULL;

    return line;
}

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf)
{
    char c;
    int i = 0;
    unsigned short line_number = 0;

    LINE *prev_line = initialize_line();
    LINE *curr_line = initialize_line();

    while (fread(&c, sizeof(char), 1, file))
    {
        if (c == '\n')
        {
            curr_line->buf_[i] = c;
            curr_line->line_num = line_number;
            line_number++;
            i = 0;

            if (text_buf->num_of_lines == 0)
            {
                prev_line = curr_line;
                text_buf->first_line = prev_line;
                text_buf->current_line = prev_line;
                curr_line = initialize_line();
                text_buf->first_line->next = curr_line;
                curr_line->prev = text_buf->first_line;
                text_buf->num_of_lines++;
            }
            else
            {
                text_buf->num_of_lines++;
                prev_line->next = curr_line;
                curr_line->prev = prev_line;
                prev_line = curr_line;
                curr_line = initialize_line();
            }

            continue;
        }

        curr_line->buf_[i] = c;
        curr_line->length++;
        i++;
    }
}
