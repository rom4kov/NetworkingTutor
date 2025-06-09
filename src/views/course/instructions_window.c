#include "../views.h"
#include "../../core/core.h"
#include <curses.h>
#include <string.h>

void print_course_instructions(APP_CONTEXT *ctx, int course_num)
{
    ctx->rp_state->course_section_data = get_course_section_data(
        ctx->db, 1, 0, &ctx->rp_state->num_of_section_items);
    ctx->current_course = ctx->courses[course_num - 1].name;
    mvwprintw(ctx->rp_state->header_win, 0, 0, "%s",
              ctx->rp_state->course_section_data[0].content);
    wattron(ctx->rp_state->right_panel, A_UNDERLINE | A_BOLD | A_BLINK);
    mvwprintw(
        ctx->rp_state->right_panel, 10,
        ((ctx->rp_state->window_width - strlen(ctx->courses[0].name)) / 2),
        "%s", ctx->courses[0].name);
    wattroff(ctx->rp_state->right_panel, A_UNDERLINE | A_BOLD | A_BLINK);
    char *section_title =
        ctx->rp_state->course_section_data[0].section_title;
    mvwprintw(ctx->rp_state->right_panel, 12,
              ((ctx->rp_state->window_width - strlen(section_title)) / 2),
              "%s", section_title);

    print_next_course_item(1, ctx->rp_state);

    char *press_space = "Press SPACE to continue";
    mvwprintw(ctx->rp_state->right_panel, LINES - 2,
              (ctx->rp_state->window_width - strlen(press_space)) / 2, "%s",
              press_space);
}

void print_next_course_item(int item, RIGHT_PANEL_STATE *rp_state)
{
    char *title = rp_state->course_section_data[item].content_title;
    char *text = rp_state->course_section_data[item].content;
    rp_state->curr_item = item;
    rp_state->curr_offset =
        item < 2 ? 0
                 : rp_state->curr_offset +
                       (strlen(text) / (rp_state->window_width - 15)) +
                       (item > 1 ? 3 : 0);
    wattron(rp_state->inner_win, A_BOLD);
    mvwprintw(rp_state->inner_win, rp_state->curr_offset, 0, "%s",
              wrap_text(title, rp_state->window_width - (COLS / 18) + 1));
    wattroff(rp_state->inner_win, A_BOLD);
    mvwprintw(rp_state->inner_win, rp_state->curr_offset + 1, 0, "%s",
              wrap_text(text, rp_state->window_width - (COLS / 14) + 1));
}
