#define _POSIX_C_SOURCE 200809L
#include "../views/views.h"
#include "core.h"
#include <curses.h>
#include <form.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

#define WINDOW_COUNT 6
#define WU COLS / 12 // WU for WIDTH_UNIT
#define CARD_WIDTH (((WU * 7) / 3) + 1)

void handle_start_input(APP_CONTEXT *ctx)
{
    int this_win;

    if (ctx->active_window_idx == 0)
    {
        handle_nav_input(ctx);
    }
    else if (ctx->active_window_idx == 1)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_DOWN:
                ctx->active_window_idx = 2;
                ctx->active_window = ctx->start_windows[2];
                draw_border(ctx->start_windows[1], 2, "Header");
                ctx->start_windows[2] = create_course_preview_card(ctx,
                    CARD_WIDTH * (ctx->active_window_idx - 2), 
                    ctx->active_window_idx, &ctx->courses[0]);
                doupdate();
                break;
            case KEY_UP:
                ctx->active_window_idx = 0;
                ctx->active_window = ctx->start_windows[0];
                focus_window(&ctx->start_windows[0], 3, "Navigation");
                focus_window(&ctx->start_windows[1], 2, "");
                doupdate();
                break;
            case KEY_RIGHT:
                ctx->active_window_idx = 5;
                ctx->active_window = ctx->start_windows[5];
                focus_window(&ctx->start_windows[1], 2, "");
                focus_window(&ctx->start_windows[5], 3, "Details");
                doupdate();
                break;
            case '\n':
                ctx->active_window_idx = 0;
                ctx->start_view_active = false;
                ctx->start_needs_redraw = false;
                // for (int i = 0; i < START_WINDOW_COUNT; i++)
                // {
                //     wclear(ctx->start_windows[i]);
                //     wnoutrefresh(ctx->start_windows[i]);
                //     delwin(ctx->start_windows[i]);
                // }
                // doupdate();
                ctx->greeter_needs_redraw = true;
                ctx->greeter_view_active = true;
                break;
        }
    }
    else if (ctx->active_window_idx == 2 || ctx->active_window_idx == 3)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_RIGHT:
                ctx->active_window_idx = (ctx->active_window_idx + 1) % WINDOW_COUNT;
                ctx->active_window = ctx->start_windows[ctx->active_window_idx];
                ctx->start_windows[ctx->active_window_idx - 1] =
                    create_course_preview_card(ctx,
                        CARD_WIDTH * (ctx->active_window_idx - 3),
                        ctx->active_window_idx - 1,
                        &ctx->courses[ctx->active_window_idx - 3]);
                doupdate();
                ctx->start_windows[ctx->active_window_idx] =
                    create_course_preview_card(ctx,
                        CARD_WIDTH * (ctx->active_window_idx - 2),
                        ctx->active_window_idx,
                        &ctx->courses[ctx->active_window_idx - 2]);
                doupdate();
                break;
            case KEY_LEFT:
                if (ctx->active_window_idx == 3)
                {
                    ctx->active_window_idx = 2;
                    ctx->active_window = ctx->start_windows[2];
                    ctx->start_windows[2] = create_course_preview_card(ctx,
                        0, 2, &ctx->courses[0]);
                    ctx->start_windows[3] = create_course_preview_card(ctx,
                        CARD_WIDTH, 3, &ctx->courses[1]);
                    doupdate();
                }
                break;
            case KEY_UP:
                this_win = ctx->active_window_idx;
                ctx->active_window_idx = 1;
                ctx->active_window = ctx->start_windows[1];
                focus_window(&ctx->start_windows[1], 3, "");
                ctx->start_windows[this_win] = create_course_preview_card(ctx,
                    CARD_WIDTH * (this_win - 2), this_win,
                    &ctx->courses[this_win - 2]);
                doupdate();
                break;
            case 10: // Enter / Return key
                this_win = ctx->active_window_idx;
                ctx->start_view_active = false;
                for (int i = 0; i < WINDOW_COUNT; i++)
                {
                    wclear(ctx->start_windows[i]);
                    wnoutrefresh(ctx->start_windows[i]);
                }
                doupdate();
                ctx->course_needs_redraw = true;
                ctx->course_view_active = true;
                ctx->current_course_id = ctx->courses[this_win - 2].id;
                ctx->current_course = strdup(ctx->courses[this_win - 2].name);
                for (int i = 1; i < WINDOW_COUNT; ++i)
                {
                    delwin(ctx->start_windows[i]);
                    wclear(ctx->start_windows[i]);
                }
                break;
        }
    }
    else if (ctx->active_window_idx == 4)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_RIGHT:
                ctx->active_window_idx = (ctx->active_window_idx + 1) % WINDOW_COUNT;
                ctx->active_window = ctx->start_windows[ctx->active_window_idx];
                ctx->start_windows[ctx->active_window_idx - 1] =
                    create_course_preview_card(ctx,
                        CARD_WIDTH * (ctx->active_window_idx - 3),
                        ctx->active_window_idx - 1,
                        &ctx->courses[2]);
                focus_window(&ctx->start_windows[5], 3, "Details");
                doupdate();
                break;
            case KEY_LEFT:
                ctx->active_window_idx = 3;
                ctx->active_window = ctx->start_windows[3];
                ctx->start_windows[3] = create_course_preview_card(ctx,
                    CARD_WIDTH, 3, &ctx->courses[1]);
                ctx->start_windows[4] = create_course_preview_card(ctx,
                    CARD_WIDTH * 2, 4, &ctx->courses[2]);
                doupdate();
                break;
            case KEY_UP:
                this_win = ctx->active_window_idx;
                ctx->active_window_idx = 1;
                ctx->active_window = ctx->start_windows[1];
                focus_window(&ctx->start_windows[1], 3, "");
                ctx->start_windows[this_win] = create_course_preview_card(ctx,
                    CARD_WIDTH * 2, this_win,
                    &ctx->courses[2]);
                doupdate();
                break;
        }
    }
    else if (ctx->active_window_idx == 5)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_LEFT:
                ctx->active_window_idx = 0;
                ctx->active_window = ctx->start_windows[0];
                focus_window(&ctx->start_windows[5], 2, "Details");
                focus_window(&ctx->start_windows[0], 3, "Navigation");
                doupdate();
                break;
        }
    }
}
