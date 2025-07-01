#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include <curses.h>
#include <stdlib.h>
#include <string.h>

void add_line_break(RIGHT_PANEL_STATE *rps, I_LINE **curr_line, int i, int *j,
                    int *k, int *line_number, int *last_space_pos,
                    bool overflow)
{
    if (overflow)
    {
        (*curr_line)->buf_[*last_space_pos] = '\n';
        *j -= *k - *last_space_pos - 1;
    }
    else
    {
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
        rps->it_buffer->num_of_lines++;
        rps->it_buffer->current_line->next = *curr_line;
        (*curr_line)->prev = rps->it_buffer->current_line;
        rps->it_buffer->current_line = *curr_line;
        *curr_line = initialize_iline();
    }
}

void read_item_into_buffer(APP_CONTEXT *ctx)
{
    for (int i = 0; i < ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section]; i++)
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

        if (ctx->rp_state->curr_section > 0 &&
            i == 0)
        {
            // curr_line->buf_ = strdup(ctx->rp_state->curr_section_title);
            // curr_line->centered = true;
            // curr_line->style = A_BOLD | A_UNDERLINE;
            // curr_line->line_num = line_number;
            // line_number++;
            //
            // ctx->rp_state->it_buffer->num_of_lines++;
            // ctx->rp_state->it_buffer->current_line->next = curr_line;
            // curr_line->prev = ctx->rp_state->it_buffer->current_line;
            // ctx->rp_state->it_buffer->current_line = curr_line;
            ctx->rp_state->it_buffer->first_line->buf_ = strdup(ctx->rp_state->curr_section_title);
            ctx->rp_state->it_buffer->first_line->centered = true;
            ctx->rp_state->it_buffer->first_line->style = A_BOLD | A_UNDERLINE;
            ctx->rp_state->it_buffer->first_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            // ctx->rp_state->it_buffer->current_line->next = curr_line;
            // curr_line->prev = ctx->rp_state->it_buffer->current_line;
            // ctx->rp_state->it_buffer->current_line = curr_line;
            curr_line = initialize_iline();

            curr_line->buf_[k] = '\n';
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->first_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->first_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
            curr_line = initialize_iline();
            // mvwprintw(ctx->rp_state->right_panel, i + 1, 2, "%i", ctx->rp_state->curr_item);
            // wrefresh(ctx->rp_state->right_panel);
        }

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
                curr_line->buf_[k] = ' ';
                curr_line->buf_[k + 1] = '\n';
                curr_line->style = SEPARATOR;
                ctx->rp_state->it_buffer->num_of_lines++;
                ctx->rp_state->it_buffer->current_line->next = curr_line;
                curr_line->prev = ctx->rp_state->it_buffer->current_line;
                ctx->rp_state->it_buffer->current_line = curr_line;
                ctx->rp_state->it_buffer->current_line->next = NULL;
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
            // curr_line = initialize_iline();
            curr_line->buf_[k] = '\n';
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
            curr_line = initialize_iline();

            curr_line->buf_ = strdup(ctx->current_course);
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

            curr_line->buf_ = strdup(ctx->rp_state->curr_section_title);
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
            ctx->rp_state->it_buffer->current_line->next = NULL;
        }
        // mvwprintw(win, text_buf->num_of_lines + 18, 16, "next curr_line %s",
        // text_buf->current_line->buf_); mvwprintw(win, 32 + i, 39, "chars %i",
        // i);
    }
    // mvwprintw(ctx->course_windows[2], 2, 3, "%s", ctx->current_course);
    // wrefresh(ctx->course_windows[2]);
}

void deallocate_it_buffer(I_TEXT_BUFFER *tbuf)
{
    if (tbuf->first_line == NULL)
        return;

    I_LINE *current_line = tbuf->first_line;
    while (current_line != NULL)
    {
        I_LINE *next = current_line->next;
        if (current_line->buf_)
        {
            free(current_line->buf_);
        }
        free(current_line);
        current_line = next;
    }
    free(tbuf);
}
