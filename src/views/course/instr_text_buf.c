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
                    bool overflow, bool *bl_point)
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

    if (i >= 0)
    {
        if (rps->course_section_data[i].section_id == 0 &&
            rps->course_section_data[i].order_num == 0)
            (*curr_line)->centered = true;
    }

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
        if ((strstr(rps->it_buffer->current_line->buf_, "•") && overflow) ||
            *bl_point)
        {
            *bl_point = true;
            (*curr_line)->buf_[0] = ' ';
            (*curr_line)->buf_[1] = ' ';
            *k += 2;
        }
    }
}

void read_item_into_buffer(APP_CONTEXT *ctx)
{
    for (int i = 0;
         i < ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section];
         i++)
    {
        int content_length =
            strlen(ctx->rp_state->course_section_data[i].content);
        int title_length =
            strlen(ctx->rp_state->course_section_data[i].content_title);
        int j, k, last_space_pos;
        j = k = last_space_pos = 0;
        int line_number = 0;
        bool overflow = false;
        bool bl_point = false;

        // LINE *prev_line = initialize_line();
        I_LINE *curr_line = initialize_iline();

        if (ctx->rp_state->curr_section > 0 && i == 0)
        {
            ctx->rp_state->it_buffer->first_line->buf_ =
                strdup(ctx->rp_state->s_metadata->title);
            ctx->rp_state->it_buffer->first_line->centered = true;
            ctx->rp_state->it_buffer->first_line->style = A_BOLD | A_UNDERLINE;
            ctx->rp_state->it_buffer->first_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            curr_line = initialize_iline();

            curr_line->buf_[k] = '\n';
            curr_line->line_num = line_number;
            line_number++;

            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->first_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->first_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
            curr_line = initialize_iline();
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
                if (ctx->rp_state->course_section_data[i].syntax_hl == true)
                    curr_line->syntax_hl = true;
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
                bl_point = false;
                if (ctx->rp_state->course_section_data[i].syntax_hl == true)
                    curr_line->syntax_hl = true;
                add_line_break(ctx->rp_state, &curr_line, i, &j, &k,
                               &line_number, &last_space_pos, overflow,
                               &bl_point);
                continue;
            }
            else if (k > ctx->rp_state->window_width - 10)
            {
                overflow = true;
                add_line_break(ctx->rp_state, &curr_line, i, &j, &k,
                               &line_number, &last_space_pos, overflow,
                               &bl_point);
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

            curr_line->buf_ = strdup(ctx->rp_state->s_metadata->title);
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
    }
}

void read_end_of_course_page_into_buffer(APP_CONTEXT *ctx)
{
    char *end_of_course_ascii = get_ascii_art(ctx->db, "course_finished_msg");
    int ascii_len = strlen(end_of_course_ascii);
    int course_len = strlen(ctx->current_course);

    char *end_of_course_msg =
        get_end_of_course_msg(ctx->db, ctx->current_course_id);
    int msg_len = strlen(end_of_course_msg);

        // literal lengths
    const size_t nl2 = 2;  // "\n\n"
    const size_t nl3 = 3;  // "\n\n\n"

    // +1 for terminating NUL
    int total = ascii_len + nl2 + course_len + nl3 + msg_len + 1;

    char *end_of_course_page_str = malloc(total);
    if (!end_of_course_page_str) return;
    end_of_course_page_str[0] = '\0';

    strncat(end_of_course_page_str, end_of_course_ascii, ascii_len);
    strncat(end_of_course_page_str, "\n\n", nl2);
    strncat(end_of_course_page_str, ctx->current_course, course_len);
    strncat(end_of_course_page_str, "\n\n\n", nl3);
    strncat(end_of_course_page_str, end_of_course_msg, msg_len);
    end_of_course_page_str[total - 1] = '\0';

    int i, j, last_space_pos, k;
    i = j = last_space_pos = k = 0;
    int line_number = 0;
    bool overflow = false;
    bool bl_point = false;
    I_LINE *curr_line = initialize_iline();

    while (i < total + 1)
    {
        if (i > 0 && end_of_course_page_str[i] == '\0')
        {
            curr_line->buf_[j] = '\n';
            if (i < (ascii_len + 1))
            {
                curr_line->centered = true;
                curr_line->style = COLOR_PAIR(4) | A_BOLD;
            }
            curr_line->line_num = line_number;
            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;

            curr_line = initialize_iline();
            curr_line->buf_[j] = ' ';
            curr_line->buf_[j + 1] = '\n';
            curr_line->style = SEPARATOR;
            ctx->rp_state->it_buffer->num_of_lines++;
            ctx->rp_state->it_buffer->current_line->next = curr_line;
            curr_line->prev = ctx->rp_state->it_buffer->current_line;
            ctx->rp_state->it_buffer->current_line = curr_line;
            ctx->rp_state->it_buffer->current_line->next = NULL;
            break;
        }
        else if (end_of_course_page_str[i] == '\n')
        {
            overflow = false;
            bl_point = false;
            if (i < (ascii_len + 20))
            {
                curr_line->centered = true;
                curr_line->style = COLOR_PAIR(4) | A_BOLD;
            }
            else if (strcmp(curr_line->buf_, ctx->current_course) == 0)
            {
                curr_line->centered = true;
                curr_line->style = A_UNDERLINE | A_BOLD;
            }
            add_line_break(ctx->rp_state, &curr_line, -1, &i, &j, &line_number,
                           &last_space_pos, overflow, &bl_point);
            continue;
        }
        else if (j > ctx->rp_state->window_width - 10)
        {
            overflow = true;
            // if (i < (ascii_len + 1))
            // {
            curr_line->centered = false;
            //     curr_line->style = COLOR_PAIR(4) | A_BOLD;
            // }
            add_line_break(ctx->rp_state, &curr_line, -1, &i, &j, &line_number,
                           &last_space_pos, overflow, &bl_point);
            continue;
        }
        else if (end_of_course_page_str[i] == ' ')
        {
            last_space_pos = j;
        }

        curr_line->buf_[j] = end_of_course_page_str[i];
        curr_line->length++;
        i++;
        j++;
    }

    wrefresh(ctx->course_windows[2]);
}

void deallocate_it_buffer(I_TEXT_BUFFER *tbuf)
{
    if (tbuf->first_line == NULL)
        return;

    I_LINE *current_line = tbuf->first_line;
    while (current_line != NULL && current_line->next != NULL)
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
