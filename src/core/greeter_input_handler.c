#include "../models/models.h"
#include "../data/data_access_layer.h"
#include <ncurses.h>
#include <menu.h>

void handle_greeter_input(APP_CONTEXT *ctx)
{
    ITEM *curr_item;

    switch (ctx->key) {
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
                ctx->start_needs_redraw = true;
                ctx->start_needs_redraw = true;
                ctx->rp_state->sections_completed = 0;
                ctx->rp_state->items_completed = 0;
                ctx->rp_state->curr_section = 0;
                ctx->rp_state->curr_item = 2;
            }
            else if (item_index(curr_item) == 1) {
                PROGRESS_DATA *pd = get_progress_data(ctx);
                ctx->rp_state->sections_completed = pd->sections_completed;
                ctx->rp_state->items_completed = pd->items_completed;
                ctx->rp_state->curr_section = pd->sections_completed;
                ctx->rp_state->curr_item = pd->items_completed + 2;
            }
            break;
    }
}
