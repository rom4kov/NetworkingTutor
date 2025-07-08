#include "../data/data_access_layer.h"
#include "../models/models.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>

void handle_greeter_input(APP_CONTEXT *ctx)
{
    ITEM *curr_item;

    switch (ctx->key)
    {
        case KEY_DOWN:
            menu_driver(ctx->greeter_menu, REQ_DOWN_ITEM);
            wrefresh(ctx->greeter_screen);
            break;
        case KEY_UP:
            menu_driver(ctx->greeter_menu, REQ_UP_ITEM);
            wrefresh(ctx->greeter_screen);
            break;
        case 10:
            curr_item = current_item(ctx->greeter_menu);
            if (item_index(curr_item) == 0)
            {
                ctx->greeter_view_active = false;
                ctx->start_view_active = true;
                ctx->start_needs_redraw = true;
                ctx->rp_state->sections_completed = 0;
                ctx->rp_state->items_completed = 0;
                ctx->rp_state->curr_section = 0;
                ctx->rp_state->curr_item = 2;
            }
            else if (item_index(curr_item) == 1)
            {
                ctx->greeter_view_active = false;
                ctx->start_view_active = false;
                ctx->start_needs_redraw = false;
                ctx->course_view_active = true;
                ctx->first_course_draw = true;
                ctx->course_needs_redraw = true;

                ctx->current_course_id = get_current_course(ctx->db, 1);
                ctx->current_course =
                    get_course_name_by_id(ctx->db, ctx->current_course_id);
                int *course_progress = get_course_progress(ctx);
                int i = 0;
                while (course_progress[i])
                {
                    ctx->rp_state->course_progress[i] = course_progress[i];
                    i++;
                }
                ctx->rp_state->curr_section = i - 1;
                ctx->rp_state->curr_item = course_progress[i - 1];
                ctx->rp_state->items_completed = course_progress[i - 1];

                get_total_items_of_sections(ctx);
                // while (total_section_items[i])
                // {
                //     ctx->rp_state->total_section_items[i] =
                //         total_section_items[i];
                //     i++;
                // }
                get_completed_sections(ctx);

                // int *pd = get_progress_data(ctx);
                // ctx->current_course_id = pd->course_id;
                // ctx->current_course =
                //     get_course_by_id(ctx->db, pd->course_id)->name;
                // ctx->rp_state->sections_completed = pd->sections_completed;
                // ctx->rp_state->items_completed = pd->items_completed;
                // ctx->rp_state->curr_section = pd->sections_completed;
                // ctx->rp_state->curr_item = pd->items_completed + 2;
            }
            else if (item_index(curr_item) == 6)
            {
                ctx->greeter_view_active = false;
                ctx->running = false;
            }
            break;
        case 'q':
        case 27:
            ctx->greeter_view_active = false;
            ctx->running = false;
            break;
    }
}
