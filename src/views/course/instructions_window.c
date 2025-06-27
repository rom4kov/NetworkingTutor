#include "../../core/core.h"
#include "../views.h"
#include <curses.h>
#include <string.h>

void print_course_instructions(APP_CONTEXT *ctx)
{
    ctx->rp_state->course_section_data = get_course_section_materials(
        ctx->db, ctx->current_course_id, ctx->rp_state->curr_section,
        &ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section]);

    ctx->rp_state->curr_section_title = (char *)get_section_title(ctx);

    read_item_into_buffer(ctx);

    // ctx->rp_state->curr_item += 1;
    print_next_course_item(ctx->rp_state);
    // mvwprintw(ctx->rp_state->inner_win, 28, 25, "%i",
    //           ctx->rp_state->it_buffer->num_of_lines);
    // mvwprintw(ctx->rp_state->right_panel, 2, 2, "%s",
    // ctx->rp_state->it_buffer->first_line->buf_);
    wrefresh(ctx->rp_state->inner_win);

    char *press_space = "Press SPACE to continue";
    if (ctx->rp_state->curr_section > 0)
        mvwprintw(
            ctx->rp_state->right_panel, LINES - 5,
            ((ctx->rp_state->window_width - strlen(press_space) + 10) / 2) - 10,
            "%s", "                                     ");
    mvwprintw(ctx->rp_state->right_panel, LINES - 5,
              (ctx->rp_state->window_width - strlen(press_space)) / 2, "%s",
              press_space);
}

void print_next_course_item(RIGHT_PANEL_STATE *rp_state)
{
    // rp_state->it_buffer->current_line = rp_state->it_buffer->first_line;
    // char *title = rp_state->it_buffer->first_line->buf_;
    I_LINE *current_line = rp_state->it_buffer->first_line;
    // char *text = rp_state->course_section_data[rp_state->curr_item].content;
    int i, j;
    j = 0;

    rp_state->lines_to_print = 0;
    // mvwprintw(rp_state->inner_win, j, 4, "%s", current_line->next->buf_);
    for (i = 0; i < rp_state->curr_item; i++)
    {
        while (current_line->style != SEPARATOR)
        {
            current_line = current_line->next;
            rp_state->lines_to_print++;
        }
        current_line = current_line->next;
        rp_state->lines_to_print++;
    }

    int offset;
    current_line = rp_state->it_buffer->first_line;
    // char *text = rp_state->course_section_data[rp_state->curr_item].content;

    if (rp_state->lines_to_print > (LINES - 8))
    {
        rp_state->lines_excess = rp_state->lines_to_print - (LINES - 8);
    }

    for (i = 0; i < (rp_state->lines_excess - rp_state->scroll_offset); i++)
        current_line = current_line->next;

    // mvwprintw(rp_state->inner_win, j, 4, "%s", current_line->next->buf_);
    for (i = 0; i < (rp_state->lines_to_print - rp_state->lines_excess); i++)
    {
        // while (current_line->style != SEPARATOR)
        // {
        if (current_line->centered)
            offset = (rp_state->window_width -
                (current_line->length < 7 ? 10 : 6) -
                strlen(current_line->buf_)) /
                2;
        else
            offset = 0;
        if (current_line->style > 0)
            wattron(rp_state->inner_win, current_line->style);
        mvwprintw(rp_state->inner_win, j, offset, "%s", current_line->buf_);
        if (current_line->style > 0)
            wattroff(rp_state->inner_win, current_line->style);

        current_line = current_line->next;
        j++;
        // }
        // current_line = current_line->next;
        // j++;
    }
}

// void print_next_course_item(RIGHT_PANEL_STATE *rp_state)
// {
//     char *title =
//         rp_state->course_section_data[rp_state->curr_item].content_title;
//     char *text = rp_state->course_section_data[rp_state->curr_item].content;
//
//     wattron(rp_state->inner_win, A_BOLD);
//     if (title && strcmp(title, "") != 0)
//     {
//         mvwprintw(rp_state->inner_win, rp_state->curr_offset, 0, "%s",
//                   wrap_text(title, rp_state->window_width - (COLS / 16) +
//                   1));
//         rp_state->curr_offset = rp_state->curr_offset + 1;
//     }
//     wattroff(rp_state->inner_win, A_BOLD);
//
//     char *wr_text = wrap_text(text, rp_state->window_width - (COLS / 16) +
//     1); mvwprintw(rp_state->inner_win, rp_state->curr_offset, 0, "%s",
//     wr_text);
//
//     for (int i = 0; i < strlen(text); i++)
//     {
//         if (wr_text[i] == '\n')
//         {
//             rp_state->curr_offset += 1;
//         }
//     }
//     // rp_state->curr_item += 1;
//     rp_state->curr_offset += 2;
// }
