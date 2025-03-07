#include "../models/models.h"
#include <ncurses.h>
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
    int i, j;
    i = j = 0;
    unsigned short line_number = 0;

    LINE *prev_line = initialize_line();
    LINE *curr_line = initialize_line();

    while (fread(&c, sizeof(char), 1, file))
    {
        while (c != ' ')
        {
            if (c == '\n')
            {
                curr_line->buf_[i][j] = c;
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

            curr_line->buf_[i][j] = c;
            curr_line->length++;
            j++;
        }

        curr_line->length++;
        i++;
    }

    // while (fread(&c, sizeof(char), 1, file))
    // {
    //     if (c == '\n')
    //     {
    //         curr_line->buf_[i] = c;
    //         curr_line->line_num = line_number;
    //         line_number++;
    //         i = 0;
    //
    //         if (text_buf->num_of_lines == 0)
    //         {
    //             prev_line = curr_line;
    //             text_buf->first_line = prev_line;
    //             text_buf->current_line = prev_line;
    //             curr_line = initialize_line();
    //             text_buf->first_line->next = curr_line;
    //             curr_line->prev = text_buf->first_line;
    //             text_buf->num_of_lines++;
    //         }
    //         else
    //         {
    //             text_buf->num_of_lines++;
    //             prev_line->next = curr_line;
    //             curr_line->prev = prev_line;
    //             prev_line = curr_line;
    //             curr_line = initialize_line();
    //         }
    //
    //         continue;
    //     }
    //
    //     curr_line->buf_[i] = c;
    //     curr_line->length++;
    //     i++;
    // }
}

void write_buffer_to_file(TEXT_BUFFER *tbuf, FILE *file, int y)
{
    tbuf->current_line = tbuf->first_line;
    while (tbuf->current_line != NULL)
    {
        fwrite(tbuf->current_line->buf_, sizeof(char),
               tbuf->current_line->length, file);
        tbuf->current_line = tbuf->current_line->next;
    }
    rewind(file);
    tbuf->current_line = tbuf->first_line;
    for (int i = 0; i < y; i++)
    {
        tbuf->current_line = tbuf->current_line->next;
    }
}
