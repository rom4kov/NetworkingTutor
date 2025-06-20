#include "../../models/models.h"
#include "../../data/data_access_layer.h"
#include <string.h>

void read_item_into_buffer(char *item, TEXT_BUFFER *text_buf)
{
    int item_length = strlen(item);
    int i, j;
    i = j = 0;
    unsigned short line_number = 0;

    LINE *prev_line = initialize_line();
    LINE *curr_line = initialize_line();

    while (i < item_length)
    {
        if (item[i] == '\n')
        {
            curr_line->buf_[i] = item[i];
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

        curr_line->buf_[i] = item[i];
        curr_line->length++;
        i++;
    }
}


