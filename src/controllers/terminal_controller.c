#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>

void handle_terminal_input(APP_CONTEXT *ctx)
{
    switch (ctx->key) {
        case KEY_UP:
            break;
        case KEY_DOWN:
            break;
        case KEY_RIGHT:
            break;
        case KEY_LEFT:
            break;
        case KEY_F(1):
            curs_set(0);
            ctx->shell->terminal_focused = false;
            break;
        default:
            // mvwprintw(ctx->terminal_window, 1, ctx->shell->curr_buf_idx + 2, "%c",
            //           ctx->key);
            // wrefresh(ctx->terminal_window);
            read_term_input_and_write_to_pty(ctx);
            break;
    }
}


