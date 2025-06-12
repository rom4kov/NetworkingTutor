#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>

#define WINDOW_COUNT 4
#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDIT_MAX WU * 7 + 4

void handle_course_input(APP_CONTEXT *ctx)
{
    if (ctx->active_window == 0)
    {
        switch (ctx->key)
        {
            case KEY_LEFT:
                menu_driver(ctx->start_menu, REQ_PREV_ITEM);
                wnoutrefresh(ctx->course_windows[0]);
                doupdate();
                break;
            case KEY_RIGHT:
                menu_driver(ctx->start_menu, REQ_NEXT_ITEM);
                wnoutrefresh(ctx->course_windows[0]);
                doupdate();
                break;
            case 9:
            case KEY_DOWN:
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[0], 2, "Navigation");
                focus_window(&ctx->course_windows[2], 3, "Editor");
                doupdate();
                break;
            case '\n':
                ctx->curr_item = current_item(ctx->start_menu);
                if (item_index(ctx->curr_item) == 0)
                {
                    ctx->course_view_active = false;
                    ctx->start_view_active = true;
                    for (int i = 0; i < WINDOW_COUNT; ++i)
                    {
                        wclear(ctx->course_windows[i]);
                    }
                    ctx->course_view_active = false;
                    ctx->start_view_active = true;
                    ctx->start_needs_redraw = true;
                    break;
                }
                else if (item_index(ctx->curr_item) == 2)
                {
                    focus_window(&ctx->start_windows[0], 2, "Navigation");
                    focus_window(&ctx->start_windows[5], 3, "Details");
                    ctx->active_window = 5;
                    focus_window(&ctx->start_windows[5], 3, "Details");
                    wmove(ctx->start_windows[5], 4, 14);
                    wnoutrefresh(ctx->start_windows[5]);
                    doupdate();
                }
                break;
        }
    }
    else if (ctx->active_window == 1 && ctx->explorer_mode)
    {
        handle_explorer_input(ctx);
    }
    else if (ctx->active_window == 1)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_RIGHT:
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[1], 2, "Explorer");
                focus_window(&ctx->course_windows[2], 3, "Editor");
                doupdate();
                break;
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->course_windows[1], 2, "Explorer");
                focus_window(&ctx->course_windows[0], 3, "Navigation");
                doupdate();
                break;
            case 10:
                ctx->explorer_mode = true;
                break;
        }
    }
    else if (ctx->active_window == 2 && ctx->editor_mode && ctx->file)
    {
        handle_editor_input(ctx);
    }
    else if (ctx->active_window == 2)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_LEFT:
                ctx->active_window = 1;
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_window(&ctx->course_windows[1], 3, "Explorer");
                doupdate();
                break;
            case KEY_RIGHT:
                ctx->active_window = 3;
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_window(&ctx->course_windows[3], 3, "Course Instructions");
                doupdate();
                break;
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->course_windows[0], 3, "Navigation");
                focus_window(&ctx->course_windows[2], 2, "Editor");
                doupdate();
                break;
            case 10:
                ctx->editor_mode = true;
                if (ctx->file)
                    curs_set(2);
                if (ctx->file)
                    wmove(ctx->edit_window, ctx->t_buffer->curr_line_nr,
                          ctx->t_buffer->current_col);
                else
                    wmove(ctx->edit_window, 0, 0);
                wrefresh(ctx->edit_window);
                break;
        }
    }
    else if (ctx->active_window == 3)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_LEFT:
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[2], 3, "Editor");
                focus_window(&ctx->course_windows[3], 2, "Course Instructions");
                doupdate();
                break;
            case ' ':
                if (ctx->rp_state->curr_item + 2 <
                    ctx->rp_state->num_of_section_items)
                {
                    print_next_course_item(ctx->rp_state->curr_item + 1,
                                           ctx->rp_state);
                    wrefresh(ctx->rp_state->inner_win);
                }
                break;
        }
    }
}
