#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include <curses.h>
#include <string.h>

void read_item_into_buffer(WINDOW *win, COURSE_SECTION *c_sec,
                           I_TEXT_BUFFER *text_buf)
// void read_item_into_buffer(APP_CONTEXT *ctx)
{
    // mvwprintw(win, text_buf->num_of_lines + 18, 16, "next curr_line %s",
    // text_buf->current_line->buf_); int item_length =
    // strlen(ctx->rp_state->course_section_data[0].content);
    int content_length = strlen(c_sec->content);
    int title_length = strlen(c_sec->content_title);
    // mvwprintw(win, 5, 3, "%i", item_length);
    // wrefresh(win);
    // int item_length = 10;
    int i, j;
    i = j = 0;
    unsigned short line_number = 0;

    // LINE *prev_line = initialize_line();
    I_LINE *curr_line = initialize_iline();

    if (title_length > 1)
    {
        while (i < title_length + 1)
        {
            if (c_sec->content_title[i] == '\0')
            {
                // curr_line->buf_[j] = '\n';
                // curr_line->buf_[j + 1] = '\n';
                // curr_line->line_num = line_number;
                // text_buf->num_of_lines++;
                // text_buf->current_line->next = curr_line;
                // curr_line->prev = text_buf->current_line;
                // text_buf->current_line = curr_line;

                // curr_line = initialize_line();
                curr_line->buf_[j] = '\n';
                curr_line->style = A_BOLD;
                text_buf->num_of_lines++;
                text_buf->current_line->next = curr_line;
                curr_line->prev = text_buf->current_line;
                text_buf->current_line = curr_line;

                curr_line = initialize_iline();
                break;
            }

            curr_line->buf_[j] = c_sec->content_title[i];
            curr_line->length++;
            i++;
            j++;
        }
    }

    i = j = 0;

    while (i < content_length + 1)
    {
        if (c_sec->content[i] == '\0')
        {
            curr_line->buf_[j] = '\n';
            if (c_sec->section_id == 0 && c_sec->order_num == 0)
                curr_line->centered = true;
            // curr_line->buf_[j + 1] = '\n';
            curr_line->line_num = line_number;
            text_buf->num_of_lines++;
            text_buf->current_line->next = curr_line;
            curr_line->prev = text_buf->current_line;
            text_buf->current_line = curr_line;

            curr_line = initialize_iline();
            curr_line->buf_[j] = '\n';
            text_buf->num_of_lines++;
            text_buf->current_line->next = curr_line;
            curr_line->prev = text_buf->current_line;
            text_buf->current_line = curr_line;
            break;
        }
        // mvwprintw(win, 1 + i, 1, "%i", c_sec->content[i]);
        else if (c_sec->content[i] == '\n' || j == 85)
        {
            // mvwprintw(win, 18 + i, 29, "line break %i", i);
            curr_line->buf_[j] = '\n';
            curr_line->line_num = line_number;
            line_number++;

            if (c_sec->section_id == 0 && c_sec->order_num == 0)
                curr_line->centered = true;

            j = 0;
            i++;

            if (text_buf->num_of_lines == 0)
            {
                text_buf->num_of_lines++;

                text_buf->first_line = curr_line;
                // prev_line = text_buf->first_line;
                text_buf->current_line = text_buf->first_line;
                // mvwprintw(win, i, 29, "first curr_line %s", curr_line->buf_);
                curr_line = initialize_iline();
                // text_buf->first_line->next = curr_line;
                // curr_line->prev = text_buf->first_line;

                // prev_line = curr_line;
                // text_buf->first_line = prev_line;
                // text_buf->current_line = prev_line;
                // curr_line = initialize_line();
                // text_buf->first_line->next = curr_line;
                // curr_line->prev = text_buf->first_line;
            }
            else
            {
                // mvwprintw(win, text_buf->num_of_lines + 18, 19, "next
                // curr_line %s", curr_line->buf_);
                text_buf->num_of_lines++;
                text_buf->current_line->next = curr_line;
                curr_line->prev = text_buf->current_line;
                text_buf->current_line = curr_line;
                curr_line = initialize_iline();
            }

            continue;
        }

        curr_line->buf_[j] = c_sec->content[i];
        curr_line->length++;
        i++;
        j++;
    }

    if (c_sec->section_id == 0 && c_sec->order_num == 0)
    {

    }
    // mvwprintw(win, text_buf->num_of_lines + 18, 16, "next curr_line %s",
    // text_buf->current_line->buf_); mvwprintw(win, 32 + i, 39, "chars %i", i);
    wrefresh(win);
}
