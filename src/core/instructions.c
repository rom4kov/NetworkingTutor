#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"

void complete_section(APP_CONTEXT *ctx)
{
    set_section_completed(ctx);
    get_completed_sections(ctx);
    ctx->rp_state->curr_item = 1;
    ctx->rp_state->curr_section += 1;
    set_items_completed(ctx);
    ctx->rp_state->lines_to_print = 0;
    ctx->rp_state->lines_excess = 0;
    ctx->rp_state->scroll_offset = 0;
    ctx->rp_state->showing_test_results = false;

    // wclear(ctx->course_windows[4]);
    // delwin(ctx->course_windows[4]);
    // ctx->course_windows[4] = create_progress_window(ctx);

    update_progress_window(ctx);

    mvwprintw(ctx->course_windows[4], 1,
              ctx->rp_state->window_width - 3, "%i",
              ctx->rp_state->curr_section);
    if (ctx->rp_state->curr_section > 0)
    {
        wclear(ctx->rp_state->inner_win);
        char blank_line[ctx->rp_state->window_width - 4];
        memset(blank_line, 32, sizeof(blank_line));
        memset(&blank_line[sizeof(blank_line) - 1], '\0', 1);

        mvwprintw(ctx->rp_state->right_panel, LINES - 5, 3,
                  "%s", blank_line);
    }
    deallocate_it_buffer(ctx->rp_state->it_buffer);
    ctx->rp_state->it_buffer = initialize_it_buffer();
    print_course_instructions(ctx);
    // log_course_instr_values(ctx);

    wnoutrefresh(ctx->rp_state->right_panel);
    wnoutrefresh(ctx->rp_state->inner_win);
    wnoutrefresh(ctx->course_windows[2]);
    wnoutrefresh(ctx->course_windows[4]);
    doupdate();
}

void complete_course(APP_CONTEXT *ctx)
{
    set_section_completed(ctx);
    get_completed_sections(ctx);
    set_course_completed(ctx);
    ctx->rp_state->curr_section += 1;
    ctx->rp_state->showing_test_results = false;
    // ctx->course_windows[4] = create_progress_window(ctx);

    update_progress_window(ctx);

    print_course_complete(ctx);
}
