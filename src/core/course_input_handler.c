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

// void handle_course_input(int *ch, WINDOW **windows, WINDOW **line_num_win,
//                          WINDOW **edit_window, bool *start_view_active,
//                          bool *course_view_active, int *active_win,
//                          bool *start_needs_redraw, MENU **start_menu,
//                          ITEM **curr_item, MENU **explorer_menu, FILE_TREE *f_tree,
//                          ITEM ***menu_items, char **filename,
//                          bool *explorer_mode, bool *editor_mode, FILE **file,
//                          TEXT_BUFFER *t_buffer, int *scroll_offset,
//                          int *lines_to_print, int *y, int *x)
void handle_course_input(APP_CONTEXT *ctx)
{
    if (ctx->active_window == 0)
    {
        switch (ctx->key)
        {
            case KEY_LEFT:
                menu_driver(ctx->start_menu, REQ_PREV_ITEM);
                wrefresh(ctx->course_windows[0]);
                break;
            case KEY_RIGHT:
                menu_driver(ctx->start_menu, REQ_NEXT_ITEM);
                wrefresh(ctx->course_windows[0]);
                break;
            case 9:
            case KEY_DOWN:
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[0], 2, "Navigation");
                focus_window(&ctx->course_windows[2], 3, "Editor");
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
                    wrefresh(ctx->start_windows[5]);
                }
                break;
        }
    }
    else if (ctx->active_window == 1 && ctx->explorer_mode)
    {
        // handle_explorer_input(
        //     *ch, t_buffer, file, filename, &ctx->course_windows[1], line_num_win,
        //     &ctx->course_windows[2], edit_window, editor_mode, explorer_mode, explorer_menu,
        //     menu_items, scroll_offset, lines_to_print, active_win);
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
                break;
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->course_windows[1], 2, "Explorer");
                focus_window(&ctx->course_windows[0], 3, "Navigation");
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
                break;
            case KEY_RIGHT:
                ctx->active_window = 3;
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_window(&ctx->course_windows[3], 3, "Course Instructions");
                break;
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->course_windows[0], 3, "Navigation");
                focus_window(&ctx->course_windows[2], 2, "Editor");
                break;
            case 10:
                ctx->editor_mode = true;
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
                break;
        }
    }
}
