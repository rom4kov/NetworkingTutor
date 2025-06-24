#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include <curses.h>
#include <string.h>

void add_line_break(RIGHT_PANEL_STATE *rps, I_LINE **curr_line, int i, int *j,
                    int *k, int *line_number, int *last_space_pos, bool overflow)
{
    if (overflow)
    {
        // (*curr_line)->buf_[*last_space_pos] =
        //     rps->course_section_data[i].content[*j];
        (*curr_line)->buf_[*last_space_pos] = '\n';
        *j -= *k - *last_space_pos - 1;
    }
    else
    {
        // (*curr_line)->buf_[*k] = rps->course_section_data[i].content[*j];
        (*curr_line)->buf_[*k] = '\n';
        *j += 1;
    }

    (*curr_line)->line_num = *line_number;
    *line_number += 1;

    if (rps->course_section_data[i].section_id == 0 &&
        rps->course_section_data[i].order_num == 0)
        (*curr_line)->centered = true;

    *k = 0;

    if (rps->it_buffer->num_of_lines == 0)
    {
        rps->it_buffer->num_of_lines++;

        rps->it_buffer->first_line = *curr_line;
        rps->it_buffer->current_line = rps->it_buffer->first_line;
        *curr_line = initialize_iline();
    }
    else
    {
        // mvwprintw(win, rps->it_buffer->num_of_lines +
        // 18, 19, "next *curr_line %s", *curr_line->buf_);
        rps->it_buffer->num_of_lines++;
        rps->it_buffer->current_line->next = *curr_line;
        (*curr_line)->prev = rps->it_buffer->current_line;
        rps->it_buffer->current_line = *curr_line;
        *curr_line = initialize_iline();
    }
}

// void read_item_into_buffer(WINDOW *win, COURSE_SECTION *c_sec,
//                            I_TEXT_BUFFER *text_buf)
void read_item_into_buffer(APP_CONTEXT *ctx)
{
    for (int i = 0; i < ctx->rp_state->num_of_section_items; i++)
    {
        int content_length =
            strlen(ctx->rp_state->course_section_data[i].content);
        int title_length =
            strlen(ctx->rp_state->course_section_data[i].content_title);
        int j, k, last_space_pos;
        j = k = last_space_pos = 0;
        int line_number = 0;
        bool overflow = false;

        // LINE *prev_line = initialize_line();
        I_LINE *curr_line = initialize_iline();

        if (title_length > 1)
        {
            while (j < title_length + 1)
            {
                if (ctx->rp_state->course_section_data[i].content_title[j] ==
                    '\0')
                {
                    curr_line->buf_[k] = '\n';
                    curr_line->style = A_BOLD;
                    ctx->rp_state->it_buffer->num_of_lines++;
                    ctx->rp_state->it_buffer->current_line->next = curr_line;
                    curr_line->prev = ctx->rp_state->it_buffer->current_line;
                    ctx->rp_state->it_buffer->current_line = curr_line;

                    curr_line = initialize_iline();
                    break;
                }

                curr_line->buf_[k] =
                    ctx->rp_state->course_section_data[i].content_title[j];
                curr_line->length++;
                j++;
                k++;
            }
        }

        j = k = 0;

        while (j < content_length + 1)
        {
            if (ctx->rp_state->course_section_data[i].content[j] == '\0')
            {
                curr_line->buf_[k] = '\n';
                if (ctx->rp_state->course_section_data[i].section_id == 0 &&
                    ctx->rp_state->course_section_data[i].order_num == 0)
                    curr_line->centered = true;
                curr_line->line_num = line_number;
                ctx->rp_state->it_buffer->num_of_lines++;
                ctx->rp_state->it_buffer->current_line->next = curr_line;
                curr_line->prev = ctx->rp_state->it_buffer->current_line;
                ctx->rp_state->it_buffer->current_line = curr_line;

                curr_line = initialize_iline();
                curr_line->buf_[k] = '\n';
                ctx->rp_state->it_buffer->num_of_lines++;
                ctx->rp_state->it_buffer->current_line->next = curr_line;
                curr_line->prev = ctx->rp_state->it_buffer->current_line;
                ctx->rp_state->it_buffer->current_line = curr_line;
                break;
            }
            else if (ctx->rp_state->course_section_data[i].content[j] == '\n')
            {
                overflow = false;
                add_line_break(ctx->rp_state, &curr_line, i, &j, &k,
                               &line_number, &last_space_pos, overflow);
                continue;
            }
            else if (k > ctx->rp_state->window_width - 10)
            {
                overflow = true;
                add_line_break(ctx->rp_state, &curr_line, i, &j, &k,
                               &line_number, &last_space_pos, overflow);
                continue;
            }
            else if (ctx->rp_state->course_section_data[i].content[j] == ' ')
            {
                last_space_pos = k;
            }

            curr_line->buf_[k] =
                ctx->rp_state->course_section_data[i].content[j];
            curr_line->length++;
            j++;
            k++;
        }

        if (ctx->rp_state->course_section_data[i].section_id == 0 &&
            ctx->rp_state->course_section_data[i].order_num == 0)
        {
            curr_line->buf_[k] = '\n';
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
            curr_line = initialize_iline();

            curr_line->buf_ = ctx->current_course;
            // curr_line->buf_ = "Current course";
            curr_line->centered = true;
            curr_line->style = A_BOLD | A_UNDERLINE;
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
            curr_line = initialize_iline();

            curr_line->buf_[k] = '\n';
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;

            curr_line = initialize_iline();

            curr_line->buf_ = ctx->rp_state->curr_section_title;
            curr_line->centered = true;
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
            curr_line = initialize_iline();

            curr_line->buf_[k] = '\n';
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
        }
        // mvwprintw(win, text_buf->num_of_lines + 18, 16, "next curr_line %s",
        // text_buf->current_line->buf_); mvwprintw(win, 32 + i, 39, "chars %i",
        // i);
    }
    // mvwprintw(ctx->course_windows[2], 2, 3, "%s", ctx->current_course);
    // wrefresh(ctx->course_windows[2]);
}
