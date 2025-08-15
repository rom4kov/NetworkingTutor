#define _XOPEN_SOURCE 500
#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include "../views.h"

#include <string.h>
#include <unistd.h>

void read_task_into_buffer(APP_CONTEXT *ctx)
{
    int content_length = strlen(ctx->rp_state->current_task);
    int j, k, last_space_pos;
    j = k = last_space_pos = 0;
    int line_number = 0;
    bool overflow = false;
    bool bl_point = false;

    I_LINE *curr_line = initialize_iline();

    // TASK TITLE
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

    j = k = 0;

    while (j < content_length + 1)
    {
        if (ctx->rp_state->current_task[j] == '\0')
        {
            curr_line->buf_[k] = '\n';
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
        else if (ctx->rp_state->current_task[j] == '\n')
        {
            overflow = false;
            bl_point = false;
            add_line_break(ctx->rp_state->course_section_data,
                           ctx->rp_state->it_buffer, &curr_line, -1, &j, &k,
                           &line_number, &last_space_pos, overflow, &bl_point);
            continue;
        }
        else if (k > ctx->rp_state->window_width - 10)
        {
            overflow = true;
            add_line_break(ctx->rp_state->course_section_data,
                           ctx->rp_state->it_buffer, &curr_line, -1, &j, &k,
                           &line_number, &last_space_pos, overflow, &bl_point);
            continue;
        }
        else if (ctx->rp_state->current_task[j] == ' ')
        {
            last_space_pos = k;
        }

        curr_line->buf_[k] = ctx->rp_state->current_task[j];
        curr_line->length++;
        j++;
        k++;
    }
}
