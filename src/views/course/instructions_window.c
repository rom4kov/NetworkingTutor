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

    print_next_course_item(ctx->rp_state);

    if (ctx->rp_state->curr_section > 0)
        mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s", "<");
    else
        mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s", " ");

    if (ctx->rp_state->curr_section < ctx->rp_state->sections_completed)
        mvwprintw(ctx->rp_state->right_panel, LINES - 5,
                  ctx->rp_state->window_width - 3, "%s", ">");
    else
        mvwprintw(ctx->rp_state->right_panel, LINES - 5,
                  ctx->rp_state->window_width - 3, "%s", " ");

    mvwprintw(ctx->rp_state->right_panel, LINES - 4,
              ctx->rp_state->window_width - 18, " %s %i of %i ", "Section",
              ctx->rp_state->curr_section + 1, 9);

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
    I_LINE *current_line = rp_state->it_buffer->first_line;
    int i, j;
    j = 0;

    rp_state->lines_to_print = 0;
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

    if (rp_state->lines_to_print > (LINES - 8))
    {
        rp_state->lines_excess = rp_state->lines_to_print - (LINES - 8);
    }

    for (i = 0; i < (rp_state->lines_excess - rp_state->scroll_offset); i++)
        current_line = current_line->next;

    for (i = 0; i < (rp_state->lines_to_print - rp_state->lines_excess); i++)
    {
        if (current_line->centered)
            offset =
                (rp_state->window_width - (current_line->length < 7 ? 10 : 6) -
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
