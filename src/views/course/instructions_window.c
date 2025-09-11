#include "../../core/core.h"
#include "../views.h"
#include <curses.h>
#include <ncurses.h>
#include <string.h>

void print_course_instructions(APP_CONTEXT *ctx)
{

    // log_course_instr_values(ctx);
    // wrefresh(ctx->course_windows[2]);

    if (ctx->rp_state->curr_section < ctx->rp_state->total_course_sections)
    {
        ctx->rp_state->course_section_data = get_course_section_materials(
            ctx->db, ctx->current_course_id, ctx->rp_state->curr_section,
            &ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section]);

        ctx->rp_state->s_metadata = get_section_metadata(ctx);

        read_items_into_buffer(ctx);
        print_next_course_item(ctx->rp_state);

        if (ctx->rp_state->showing_test_results)
        {
            mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s",
                      "< Back to section");
        }
        else
        {
            if (ctx->rp_state->curr_section > 0)
                mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s", "<");
            else
                mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s", " ");
        }

        if (ctx->rp_state->curr_section < ctx->rp_state->sections_completed)
            mvwprintw(ctx->rp_state->right_panel, LINES - 5,
                      ctx->rp_state->window_width - 3, "%s", ">");
        else
            mvwprintw(ctx->rp_state->right_panel, LINES - 5,
                      ctx->rp_state->window_width - 3, "%s", " ");

        mvwprintw(ctx->rp_state->right_panel, LINES - 4,
                  ctx->rp_state->window_width - 18, " %s %i of %i ", "Section",
                  ctx->rp_state->curr_section + 1,
                  ctx->rp_state->total_course_sections);
    }
    else
    {
        ctx->rp_state->showing_end_of_course_page = true;
        print_course_complete(ctx);
    }
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

    int offset = 0;
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
        {
            offset =
                (rp_state->window_width - (current_line->length < 7 ? 10 : 6) -
                 strlen(current_line->buf_)) /
                2;
        }
        else
            offset = 0;
        if (current_line->style > 0)
        {
            wattron(rp_state->inner_win, current_line->style);
            mvwprintw(rp_state->inner_win, j, offset, "%s", current_line->buf_);
            wattroff(rp_state->inner_win, current_line->style);
        }
        else if (current_line->style == 0 && current_line->syntax_hl == false)
        {
            mvwprintw(rp_state->inner_win, j, offset, "%s", current_line->buf_);
        }
        else if (current_line->syntax_hl == true)
        {
            print_line((LINE *)current_line, j, &rp_state->inner_win);
        }

        current_line = current_line->next;
        j++;
    }

    if (rp_state->curr_section < rp_state->total_course_sections)
        print_press_msg(rp_state);
}

void print_press_msg(RIGHT_PANEL_STATE *rps)
{
    char blank_line[rps->window_width - 8];
    memset(blank_line, 32, sizeof(blank_line));
    memset(&blank_line[sizeof(blank_line) - 1], '\0', 1);

    mvwprintw(rps->right_panel, LINES - 5, 3, "%s", blank_line);

    if (rps->curr_item < rps->num_of_section_items[rps->curr_section])
    {
        rps->ready_to_test = false;
        char *press_space = "Press SPACE to continue";
        mvwprintw(rps->right_panel, LINES - 5,
                  ((rps->window_width - strlen(press_space) + 10) / 2) - 10,
                  "%s", "                                     ");
        mvwprintw(rps->right_panel, LINES - 5,
                  (rps->window_width - strlen(press_space)) / 2, "%s",
                  press_space);
    }
    else if (rps->curr_item == rps->num_of_section_items[rps->curr_section] &&
             rps->curr_section >= rps->sections_completed)
    {
        if (rps->s_metadata->has_test && rps->s_metadata->has_separate_task)
        {
            char *press_enter = "Press t to see your task";
            mvwprintw(rps->right_panel, LINES - 5,
                      (rps->window_width - strlen(press_enter)) / 2, "%s",
                      press_enter);
        }
        else if (rps->s_metadata->has_test &&
                 !rps->s_metadata->has_separate_task)
        {
            rps->ready_to_test = true;
            char *press_enter = "Press s to submit your task";
            mvwprintw(rps->right_panel, LINES - 5,
                      (rps->window_width - strlen(press_enter)) / 2, "%s",
                      press_enter);
        }
        else
        {
            char *press_enter = "Press ENTER to go to next section";
            mvwprintw(rps->right_panel, LINES - 5,
                      (rps->window_width - strlen(press_enter)) / 2, "%s",
                      press_enter);
        }
    }
    wrefresh(rps->right_panel);
}

void print_course_complete(APP_CONTEXT *ctx)
{
    werase(ctx->rp_state->inner_win);
    werase(ctx->rp_state->right_panel);

    focus_window(&ctx->rp_state->right_panel, ctx->active_window_idx == 3 ? 3 : 2,
                 "Course instructions");

    ctx->rp_state->lines_excess = 0;

    read_end_of_course_page_into_buffer(ctx);

    ctx->rp_state->curr_item = 1;

    print_next_course_item(ctx->rp_state);

    wnoutrefresh(ctx->rp_state->right_panel);
    wnoutrefresh(ctx->rp_state->inner_win);
    doupdate();
}
