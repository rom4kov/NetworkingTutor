#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <ncurses.h>

void handle_inactive_editor_input(APP_CONTEXT *ctx)
{
        switch (ctx->key)
        {
        case KEY_UP:
            ctx->active_window_idx = 0;
            ctx->active_window = ctx->course_windows[0];
            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_window(&ctx->course_windows[0], 3, "Navigation");
            doupdate();
            break;
        case KEY_DOWN:
            if (ctx->shell->terminal_active)
            {
                ctx->active_window_idx = SHELL_WINDOW_IDX;
                ctx->active_window = ctx->terminal_window;
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_window(&ctx->terminal_window, 3, "Terminal");
                doupdate();
            }
            break;
        case 9:
        case KEY_LEFT:
            ctx->active_window_idx = 1;
            ctx->active_window = ctx->course_windows[1];
            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_window(&ctx->course_windows[1], 3, "Explorer");
            doupdate();
            break;
        case KEY_RIGHT:
            ctx->active_window_idx = 3;
            ctx->active_window = ctx->course_windows[3];
            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_instructions_window(ctx->rp_state, 3, "Course Instructions");
            wnoutrefresh(ctx->rp_state->inner_win);
            doupdate();
            break;
        case 't':
            if (!ctx->shell->terminal_active)
            {
                ctx->shell->terminal_active = true;
                ctx->shell->terminal_focused = true;
                ctx->active_window_idx = SHELL_WINDOW_IDX;
                recreate_editor_windows(ctx);

                ctx->active_window_idx = SHELL_WINDOW_IDX;
                toggle_terminal(ctx);

                print_term_buf(ctx->shell->term_inner_win,
                               ctx->shell->term_buffer);
                wmove(ctx->shell->term_inner_win, 0, 2);

                update_panels();
                curs_set(2);
                wnoutrefresh(ctx->course_windows[2]);
                wnoutrefresh(ctx->shell->term_inner_win);

                doupdate();
            }
            else
            {
                curs_set(0);
                ctx->shell->terminal_active = false;
                ctx->shell->terminal_focused = false;
                ctx->active_window_idx = 2;
                recreate_editor_windows(ctx);

                ctx->active_window_idx = 2;
                toggle_terminal(ctx);

                wmove(ctx->edit_window,
                      ctx->t_buffer->curr_line_nr - ctx->scroll_offset,
                      ctx->t_buffer->current_col);
                wnoutrefresh(ctx->edit_window);
                wnoutrefresh(ctx->terminal_window);
                wnoutrefresh(ctx->course_windows[2]);
                update_panels();
                doupdate();
            }
            break;
        case 10:
            ctx->editor_mode = true;
            if (ctx->file)
                curs_set(2);
            if (ctx->file)
            {
                if (ctx->y > ctx->editor_height - 5)
                {
                    ctx->t_buffer->current_line = ctx->t_buffer->first_line;
                    for (int i = 0; i < ctx->t_buffer->curr_line_nr; i++)
                    {
                        ctx->t_buffer->current_line =
                            ctx->t_buffer->current_line->next;
                    }
                }
                wmove(ctx->edit_window,
                      ctx->t_buffer->curr_line_nr - ctx->scroll_offset,
                      ctx->t_buffer->current_col);
            }
            else
                wmove(ctx->edit_window, 0, 0);
            wrefresh(ctx->edit_window);
            break;
        }
}
