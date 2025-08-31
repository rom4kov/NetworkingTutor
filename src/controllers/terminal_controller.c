#include "../../ntutor.h"
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <signal.h>

void handle_terminal_input(APP_CONTEXT *ctx)
{
    switch (ctx->key)
    {
        case KEY_UP:
            scroll_up(ctx);
            break;
        case KEY_DOWN:
            scroll_down(ctx);
            break;
        case KEY_RIGHT:
            move_cursor_right(ctx);
            break;
        case KEY_LEFT:
            move_cursor_left(ctx);
            break;
        case KEY_BACKSPACE:
            delete_char_with_back_space(ctx);
            break;
        case '\n':
            submit_command(ctx);
            break;
        case KEY_F(1):
            curs_set(0);
            ctx->shell->terminal_focused = false;
            break;
        case KEY_F(2):
            if (ctx->shell->executable_running)
            {
                ctx->shell->executable_running = false;
                kill(-(ctx->shell->child_pid), SIGKILL);
                run_output_funcs(ctx, "");
            }
            break;
        default:
            print_term_input(ctx);
            break;
    }
}
