#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"

void handle_progress_input(APP_CONTEXT *ctx)
{
    if (ctx->active_window == 0)
    {
        handle_nav_input(ctx);
    }
    else if (ctx->active_window == 1) {
        switch (ctx->key)
        {
            case 9:
            case KEY_UP:
                ctx->active_window = 0;
                draw_progress_border(ctx->progress_windows[1], 2, "");
                ctx->progress_windows[0] =
                    create_navigation_window(&ctx->active_window, &ctx->start_menu);
                doupdate();
                break;
        }
    }
}
