#include "../../models/models.h"
#include "../../data/data_access_layer.h"
#include <curses.h>
#include <string.h>

void read_item_into_buffer(WINDOW *win, COURSE_SECTION *c_sec, TEXT_BUFFER *text_buf)
// void read_item_into_buffer(APP_CONTEXT *ctx)
{
    // int item_length = strlen(ctx->rp_state->course_section_data[0].content);
    int item_length = strlen(c_sec->content);
    mvwprintw(win, 5, 3, "%i", item_length);
    wrefresh(win);
    // int item_length = 10;
    int i, j;
    i = j = 0;
    unsigned short line_number = 0;

    LINE *prev_line = initialize_line();
    LINE *curr_line = initialize_line();

    while (i < item_length - 5)
    {
        mvwprintw(win, 1 + i, 1, "%i", c_sec->content[i]);
        wrefresh(win);
        if (c_sec->content[i] == '\n')
        {
            mvwprintw(win, 18 + i, 9, "line break %i", i);
            wrefresh(win);
        }
        if (c_sec->content[i] == '\n')
        {
            mvwprintw(win, 18 + i, 29, "line break %i", i);
            wrefresh(win);
            curr_line->buf_[j] = '\n';
            curr_line->line_num = line_number;
            line_number++;
            j = 0;
            i++;

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
                text_buf->current_line = prev_line;
                curr_line = initialize_line();
            }

            continue;
        }

        curr_line->buf_[j] = c_sec->content[i];
        curr_line->length++;
        i++;
        j++;
    }
}


