#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <ncurses.h>

void handle_progress_input(APP_CONTEXT *ctx)
{
    if (ctx->active_window == 0)
    {
        handle_nav_input(ctx);
    }
    else if (ctx->active_window == 1 && ctx->user_form_active)
    {
        switch (ctx->key)
        {
            case KEY_DOWN:
                form_driver(ctx->user_form, REQ_DOWN_FIELD);
                form_driver(ctx->user_form, REQ_END_LINE);
                wnoutrefresh(ctx->progress_windows[1]);
                doupdate();
                break;
            case KEY_UP:
                form_driver(ctx->user_form, REQ_UP_FIELD);
                form_driver(ctx->user_form, REQ_END_LINE);
                wnoutrefresh(ctx->progress_windows[1]);
                doupdate();
                break;
            case 9:
                form_driver(ctx->user_form, REQ_NEXT_FIELD);
                wrefresh(ctx->progress_windows[1]);
                break;
            case 263: // Backspace
                form_driver(ctx->user_form, REQ_VALIDATION);
                FIELD *current = current_field(ctx->user_form);
                char *buf = field_buffer(current, 0);
                trim(&buf);
                if (buf && strlen(buf) > 0)
                {
                    form_driver(ctx->user_form, REQ_DEL_PREV);
                    wrefresh(ctx->progress_windows[1]);
                }
                break;
            case '\n':
                form_driver(ctx->user_form, REQ_VALIDATION);
                char *buf1 = field_buffer(&ctx->user_form_fields[0], 0);
                trim(&buf1);
                update_user(ctx->db, 1, buf1);
                curs_set(0);
                ctx->progress_view_active = true;
                ctx->progress_needs_redraw = true;
                ctx->user_form_active = false;
                // unpost_form(ctx->user_form);
                free_form(ctx->user_form);
                free_field(&ctx->user_form_fields[0]);
                free_field(&ctx->user_form_fields[1]);
                break;
            case KEY_F(1):
                curs_set(0);
                mvwprintw(ctx->progress_windows[1], 1, 1, "%s", "F1 pressed");
                ctx->user_form_active = false;
                wrefresh(ctx->progress_windows[1]);
                break;
            default:
                form_driver(ctx->user_form, ctx->key);
                wrefresh(ctx->progress_windows[1]);
                break;
        }
    }
    else if (ctx->active_window == 1) {
        switch (ctx->key)
        {
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->progress_windows[1], 2, "");
                ctx->progress_windows[0] =
                    create_navigation_window(&ctx->active_window, &ctx->start_menu);
                doupdate();
                break;
            case 9:
            case KEY_DOWN:
                ctx->active_window = 2;
                curs_set(0);
                focus_window(&ctx->progress_windows[1], 2, "");
                focus_window(&ctx->progress_windows[2], 3, "");
                doupdate();
                break;
            case KEY_RIGHT:
                ctx->active_window = 3;
                curs_set(0);
                focus_window(&ctx->progress_windows[1], 2, "");
                draw_progress_border(ctx->progress_windows[3], 3, "");
                create_your_courses_window(ctx, ctx->progress_windows[3]);
                print_completed_courses(ctx);
                doupdate();
                break;
            case '\n':
                ctx->user_form_active = true;
                curs_set(1);
                break;
        }
    }
    else if (ctx->active_window == 2) {
        switch (ctx->key)
        {
            case KEY_UP:
                ctx->active_window = 1;
                focus_window(&ctx->progress_windows[2], 2, "");
                focus_window(&ctx->progress_windows[1], 3, "");
                doupdate();
                break;
            case KEY_RIGHT:
                ctx->active_window = 3;
                focus_window(&ctx->progress_windows[2], 2, "");
                draw_progress_border(ctx->progress_windows[3], 3, "");
                create_your_courses_window(ctx, ctx->progress_windows[3]);
                print_completed_courses(ctx);
                doupdate();
                break;
        }
    }
    else if (ctx->active_window == 3) {
        switch (ctx->key)
        {
            case KEY_LEFT:
                ctx->active_window = 1;
                draw_progress_border(ctx->progress_windows[3], 2, "");
                create_your_courses_window(ctx, ctx->progress_windows[3]);
                print_completed_courses(ctx);
                focus_window(&ctx->progress_windows[1], 3, "");
                doupdate();
                break;
            case KEY_UP:
                ctx->active_window = 0;
                draw_progress_border(ctx->progress_windows[3], 2, "");
                create_your_courses_window(ctx, ctx->progress_windows[3]);
                print_completed_courses(ctx);
                ctx->progress_windows[0] =
                    create_navigation_window(&ctx->active_window, &ctx->start_menu);
                doupdate();
                break;
        }
    }
}
