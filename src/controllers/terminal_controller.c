#include "../controllers/controllers.h"
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
            move_cursor_right(ctx);
            break;
        case KEY_LEFT:
            move_cursor_left(ctx);
            break;
        case '\n':
            submit_command(ctx);
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


