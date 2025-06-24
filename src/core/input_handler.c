#include "../views/start/user_form.h"
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
    FORM *user_form = NULL;
    FIELD *fields[3];
    ITEM *curr_item;
    int this_win;

    if (ctx->active_window == 0)
    {
        switch (ctx->key)
        {
            case KEY_LEFT:
                menu_driver(ctx->start_menu, REQ_PREV_ITEM);
                wrefresh(ctx->start_windows[0]);
                break;
            case KEY_RIGHT:
                menu_driver(ctx->start_menu, REQ_NEXT_ITEM);
                wrefresh(ctx->start_windows[0]);
                break;
            case 9:
            case KEY_DOWN:
                ctx->active_window = 1;
                focus_window(&ctx->start_windows[0], 2, "Navigation");
                focus_window(&ctx->start_windows[1], 3, "");
                doupdate();
                break;
            case '\n':
                curr_item = current_item(ctx->start_menu);
                if (item_index(curr_item) == 2)
                {
                    focus_window(&ctx->start_windows[0], 2, "Navigation");
                    focus_window(&ctx->start_windows[5], 3, "Details");
                    ctx->active_window = 5;
                    create_user_form(&ctx->start_windows[5], &user_form,
                                     fields);
                    focus_window(&ctx->start_windows[5], 3, "Details");
                    wmove(ctx->start_windows[5], 4, 14);
                    wnoutrefresh(ctx->start_windows[5]);
                    doupdate();
                }
                break;
        }
    }
    else if (ctx->active_window == 1)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_DOWN:
                ctx->active_window = 2;
                draw_border(ctx->start_windows[1], 2, "Header");
                ctx->start_windows[2] = create_course_preview_card(
                    CARD_WIDTH * (ctx->active_window - 2), &ctx->active_window,
                    ctx->active_window, &ctx->courses[0]);
                doupdate();
                break;
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->start_windows[0], 3, "Navigation");
                focus_window(&ctx->start_windows[1], 2, "");
                doupdate();
                break;
            case KEY_RIGHT:
                ctx->active_window = 5;
                focus_window(&ctx->start_windows[1], 2, "");
                focus_window(&ctx->start_windows[5], 3, "Details");
                doupdate();
                break;
        }
    }
    else if (ctx->active_window == 2 || ctx->active_window == 3)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_RIGHT:
                ctx->active_window = (ctx->active_window + 1) % WINDOW_COUNT;
                ctx->start_windows[ctx->active_window - 1] =
                    create_course_preview_card(
                        CARD_WIDTH * (ctx->active_window - 3),
                        &ctx->active_window, ctx->active_window - 1,
                        &ctx->courses[ctx->active_window - 3]);
                doupdate();
                ctx->start_windows[ctx->active_window] =
                    create_course_preview_card(
                        CARD_WIDTH * (ctx->active_window - 2),
                        &ctx->active_window, ctx->active_window,
                        &ctx->courses[ctx->active_window - 2]);
                doupdate();
                break;
            case KEY_LEFT:
                if (ctx->active_window == 3)
                {
                    ctx->active_window = 2;
                    ctx->start_windows[2] = create_course_preview_card(
                        0, &ctx->active_window, 2, &ctx->courses[0]);
                    ctx->start_windows[3] = create_course_preview_card(
                        CARD_WIDTH, &ctx->active_window, 3, &ctx->courses[1]);
                    doupdate();
                }
                break;
            case KEY_UP:
                this_win = ctx->active_window;
                ctx->active_window = 1;
                focus_window(&ctx->start_windows[1], 3, "");
                ctx->start_windows[this_win] = create_course_preview_card(
                    CARD_WIDTH * (this_win - 2), &ctx->active_window, this_win,
                    &ctx->courses[this_win - 2]);
                doupdate();
                break;
            case 10: // Enter / Return key
                this_win = ctx->active_window;
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
                ctx->current_course = ctx->courses[this_win - 2].name;
                for (int i = 1; i < WINDOW_COUNT; ++i)
                {
                    delwin(ctx->start_windows[i]);
                    wclear(ctx->start_windows[i]);
                }
                break;
        }
    }
    else if (ctx->active_window == 4)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_RIGHT:
                ctx->active_window = (ctx->active_window + 1) % WINDOW_COUNT;
                ctx->start_windows[ctx->active_window - 1] =
                    create_course_preview_card(
                        CARD_WIDTH * (ctx->active_window - 3),
                        &ctx->active_window, ctx->active_window - 1,
                        &ctx->courses[2]);
                focus_window(&ctx->start_windows[5], 3, "Details");
                doupdate();
                break;
            case KEY_LEFT:
                ctx->active_window = 3;
                ctx->start_windows[3] = create_course_preview_card(
                    CARD_WIDTH, &ctx->active_window, 3, &ctx->courses[1]);
                ctx->start_windows[4] = create_course_preview_card(
                    CARD_WIDTH * 2, &ctx->active_window, 4, &ctx->courses[2]);
                doupdate();
                break;
            case KEY_UP:
                this_win = ctx->active_window;
                ctx->active_window = 1;
                focus_window(&ctx->start_windows[1], 3, "");
                ctx->start_windows[this_win] = create_course_preview_card(
                    CARD_WIDTH * 2, &ctx->active_window, this_win,
                    &ctx->courses[2]);
                doupdate();
                break;
        }
    }
    else if (ctx->active_window == 5 && user_form)
    {
        switch (ctx->key)
        {
            case KEY_DOWN:
                form_driver(user_form, REQ_DOWN_FIELD);
                form_driver(user_form, REQ_END_LINE);
                wnoutrefresh(ctx->start_windows[5]);
                doupdate();
                break;
            case KEY_UP:
                form_driver(user_form, REQ_UP_FIELD);
                form_driver(user_form, REQ_END_LINE);
                wnoutrefresh(ctx->start_windows[5]);
                doupdate();
                break;
            case 9:
                form_driver(user_form, REQ_NEXT_FIELD);
                wrefresh(ctx->start_windows[5]);
                break;
            case 263: // Backspace
                form_driver(user_form, REQ_VALIDATION);
                FIELD *current = current_field(user_form);
                char *buf = field_buffer(current, 0);
                trim(&buf);
                if (buf && strlen(buf) > 0)
                {
                    form_driver(user_form, REQ_DEL_PREV);
                    wrefresh(ctx->start_windows[5]);
                }
                break;
            case '\n':
                form_driver(user_form, REQ_VALIDATION);
                char *buf1 = field_buffer(fields[0], 0);
                char *buf2 = field_buffer(fields[1], 0);
                update_user(ctx->db, 1, buf1, buf2);
                curs_set(0);
                ctx->start_windows[5] = create_right_side_panel(ctx, "Details");
                wrefresh(ctx->start_windows[5]);
                ctx->start_view_active = false;
                ctx->start_needs_redraw = true;
                unpost_form(user_form);
                free_form(user_form);
                free_field(fields[0]);
                free_field(fields[1]);
                break;
            default:
                form_driver(user_form, ctx->key);
                wrefresh(ctx->start_windows[5]);
                break;
        }
    }
    else if (ctx->active_window == 5)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_LEFT:
                ctx->active_window = 0;
                focus_window(&ctx->start_windows[5], 2, "Details");
                focus_window(&ctx->start_windows[0], 3, "Navigation");
                doupdate();
                break;
        }
    }
}
