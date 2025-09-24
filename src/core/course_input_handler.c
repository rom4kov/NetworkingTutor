#include "../../ntutor.h"
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../course_tests/tests.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <stdbool.h>
#include <unistd.h>

void handle_course_input(APP_CONTEXT *ctx)
{
    if (ctx->active_window_idx == 0)
    {
        handle_nav_input(ctx);
    }
    else if (ctx->active_window_idx == 1 && ctx->explorer_mode)
    {
        handle_explorer_input(ctx);
    }
    else if (ctx->active_window_idx == 1)
    {
        switch (ctx->key)
        {
        case 9:
        case KEY_RIGHT:
            ctx->active_window_idx = 2;
            ctx->active_window = ctx->course_windows[2];
            focus_window(&ctx->course_windows[1], 2, "Explorer");
            focus_window(&ctx->course_windows[2], 3, "Editor");
            doupdate();
            break;
        case KEY_UP:
            ctx->active_window_idx = 0;
            ctx->active_window = ctx->course_windows[0];
            focus_window(&ctx->course_windows[1], 2, "Explorer");
            focus_window(&ctx->course_windows[0], 3, "Navigation");
            doupdate();
            break;
        case 10:
            // free(ctx->prev_dir);
            // ctx->prev_dir = get_cwd();
            // chdir(ctx->user_data->home_dir);
            ctx->explorer_mode = true;
            break;
        }
    }
    else if (ctx->active_window_idx == 2 && ctx->editor_mode && ctx->file)
    {
        handle_editor_input(ctx);
    }
    else if (ctx->active_window_idx == 2)
    {
        handle_inactive_editor_input(ctx);
    }
    else if (ctx->active_window_idx == 3)
    {
        handle_instr_window_input(ctx);
    }
    else if (ctx->active_window_idx == SHELL_WINDOW_IDX &&
             ctx->shell->terminal_focused)
    {
        handle_terminal_input(ctx);
    }
    else if (ctx->active_window_idx == SHELL_WINDOW_IDX)
    {
        switch (ctx->key)
        {
        case KEY_UP:
            curs_set(0);
            ctx->active_window_idx = 2;
            ctx->active_window = ctx->course_windows[2];
            focus_window(&ctx->terminal_window, 2, "Terminal");
            focus_window(&ctx->course_windows[2], 3, "Editor");
            doupdate();
            break;
        case KEY_LEFT:
            curs_set(0);
            ctx->active_window_idx = 1;
            ctx->active_window = ctx->course_windows[1];
            focus_window(&ctx->terminal_window, 2, "Terminal");
            focus_window(&ctx->course_windows[1], 3, "Explorer");
            doupdate();
            break;
        case KEY_RIGHT:
            curs_set(0);
            ctx->active_window_idx = 3;
            ctx->active_window = ctx->course_windows[3];
            focus_window(&ctx->terminal_window, 2, "Terminal");
            focus_window(&ctx->course_windows[3], 3, "Course Details");
            doupdate();
            break;
        case '\n':
            ctx->shell->terminal_focused = true;

            chdir(ctx->shell_local_cwd);

            int nol = ctx->shell->term_buffer->num_of_lines;
            wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
                  ctx->shell->term_buffer->current_col);
            curs_set(2);
            wrefresh(ctx->shell->term_inner_win);
            break;
        case 't':
            curs_set(0);
            ctx->shell->terminal_active = false;
            ctx->active_window_idx = 2;
            ctx->editor_mode = false;
            recreate_editor_windows(ctx);
            toggle_terminal(ctx);
            wmove(ctx->edit_window,
                  ctx->t_buffer->curr_line_nr - ctx->scroll_offset,
                  ctx->t_buffer->current_col);
            wrefresh(ctx->edit_window);
            break;
        }
    }
}
