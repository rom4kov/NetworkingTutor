#include "../../ntutor.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"

void go_to_course_by_id(APP_CONTEXT *ctx, int course_id)
{
    wclear(ctx->greeter_screen);
    wrefresh(ctx->greeter_screen);
    delwin(ctx->greeter_screen);
    delwin(ctx->greeter_ascii_window);
    ctx->greeter_screen = NULL;
    ctx->greeter_view_active = false;
    ctx->greeter_needs_redraw = false;
    ctx->start_view_active = false;
    ctx->start_needs_redraw = false;
    ctx->course_view_active = true;
    ctx->first_course_draw = true;
    ctx->course_needs_redraw = true;
    // for (int i = 0; i < START_WINDOW_COUNT; i++)
    // {
    //     wclear(ctx->start_windows[i]);
    //     wrefresh(ctx->start_windows[i]);
    //     delwin(ctx->start_windows[i]);
    // }

    ctx->current_user_id = 1;
    ctx->user_data = get_user_data(ctx->db, ctx->current_user_id);
    ctx->current_course_id = course_id;
    ctx->current_course =
        get_course_name_by_id(ctx->db, ctx->current_course_id);

    for (int i = 0; i < MAX_COURSE_SECTIONS; i++)
    {
        ctx->rp_state->course_progress[i] = 0;
    }

    deallocate_it_buffer(ctx->rp_state->it_buffer);
    ctx->rp_state->it_buffer = initialize_it_buffer();

    get_course_progress(ctx);

    get_completed_sections(ctx);

    get_total_course_sections(ctx);

    ctx->rp_state->curr_section =
        ctx->rp_state->sections_completed;
    ctx->rp_state->curr_item =
        ctx->rp_state
        ->course_progress[ctx->rp_state->sections_completed] +
        (ctx->rp_state->curr_section == 0 ? 1 : 0);
    ctx->rp_state->items_completed =
        ctx->rp_state
        ->course_progress[ctx->rp_state->sections_completed -
        1];

}
