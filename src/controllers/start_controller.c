#include "../../ntutor.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>

void go_to_course_by_id(APP_CONTEXT *ctx, int course_id)
{
    if (ctx->greeter_view_active)
    {
        ctx->greeter_view_active = false;
        ctx->greeter_needs_redraw = false;
        for (int i = 0; i < GREETER_WINDOW_COUNT; i++)
        {
            wclear(ctx->greeter_windows[i]);
            delwin(ctx->greeter_windows[i]);
        }
        delwin(ctx->greeter_ascii_window);
    }
    else if (ctx->start_view_active)
    {
        ctx->start_view_active = false;
        ctx->start_needs_redraw = false;
    }
    else if (ctx->progress_view_active)
    {
        ctx->progress_view_active = false;
        ctx->progress_needs_redraw = false;
    }
    else if (ctx->all_courses_view_active)
    {
        ctx->all_courses_view_active = false;
        ctx->all_courses_needs_redraw = false;
    }
    else if (ctx->keybindings_view_active)
    {
        ctx->keybindings_view_active = false;
        ctx->keybindings_needs_redraw = false;
    }

    ctx->course_view_active = true;
    ctx->course_needs_redraw = true;

    ctx->current_course_id = course_id;
    ctx->current_course = ctx->courses[course_id - 1].name;

    for (int i = 0; i < MAX_COURSE_SECTIONS; i++)
    {
        ctx->rp_state->course_progress[i] = 0;
    }

    ctx->rp_state->lines_excess = 0;
    ctx->rp_state->lines_to_print = 0;

    deallocate_it_buffer(ctx->rp_state->it_buffer);
    ctx->rp_state->it_buffer = initialize_it_buffer();

    get_course_progress(ctx);

    get_completed_sections(ctx);

    get_total_course_sections(ctx);

    ctx->rp_state->curr_section = ctx->rp_state->sections_completed;
    ctx->rp_state->curr_item =
        ctx->rp_state->course_progress[ctx->rp_state->sections_completed] +
        (ctx->rp_state->curr_section == 0 ? 1 : 0);
    if (ctx->rp_state->curr_item < 2)
        ctx->rp_state->curr_item = ctx->rp_state->curr_section == 0 ? 2 : 1;
    if (ctx->rp_state->sections_completed < 1)
    {
        ctx->rp_state->items_completed = ctx->rp_state->course_progress[0];
    }
    else
    {
        ctx->rp_state->items_completed =
            ctx->rp_state
                ->course_progress[ctx->rp_state->sections_completed - 1];
    }
    // log_course_instr_values(ctx);
    // wrefresh(ctx->course_windows[2]);
}
