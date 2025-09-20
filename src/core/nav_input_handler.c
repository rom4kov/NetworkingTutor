#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>


void handle_nav_input(APP_CONTEXT *ctx)
{
    switch (ctx->key)
    {
        case KEY_LEFT:
            menu_driver(ctx->start_menu, REQ_PREV_ITEM);
            if (ctx->start_view_active)
            {
                wnoutrefresh(ctx->start_windows[0]);
            }
            else if (ctx->course_view_active) {
                wnoutrefresh(ctx->course_windows[0]);
            }
            else if (ctx->progress_view_active) {
                wnoutrefresh(ctx->progress_windows[0]);
            }
            else if (ctx->all_courses_view_active) {
                wnoutrefresh(ctx->all_courses_windows[0]);
            }
            doupdate();
            break;
        case KEY_RIGHT:
            menu_driver(ctx->start_menu, REQ_NEXT_ITEM);
            if (ctx->start_view_active)
            {
                wnoutrefresh(ctx->start_windows[0]);
            }
            else if (ctx->course_view_active) {
                wnoutrefresh(ctx->course_windows[0]);
            }
            else if (ctx->progress_view_active) {
                wnoutrefresh(ctx->progress_windows[0]);
            }
            else if (ctx->all_courses_view_active) {
                wnoutrefresh(ctx->all_courses_windows[0]);
            }
            doupdate();
            break;
        case 9:
        case KEY_DOWN:
            if (ctx->start_view_active)
            {
                ctx->active_window_idx = 1;
                ctx->active_window = ctx->start_windows[1];
                focus_window(&ctx->start_windows[0], 2, "Navigation");
                focus_window(&ctx->start_windows[1], 3, "");
            }
            else if (ctx->course_view_active) {
                ctx->active_window_idx = 2;
                ctx->active_window = ctx->course_windows[2];
                focus_window(&ctx->course_windows[0], 2, "Navigation");
                focus_window(&ctx->course_windows[2], 3, "Editor");
            }
            else if (ctx->progress_view_active) {
                ctx->active_window_idx = 1;
                ctx->active_window = ctx->progress_windows[1];
                focus_window(&ctx->progress_windows[0], 2, "Navigation");
                focus_window(&ctx->progress_windows[1], 3, "");
            }
            doupdate();
            break;
        case '\n':
            ctx->curr_item = current_item(ctx->start_menu);
            ctx->curr_nav_item = item_index(ctx->curr_item);
            ctx->active_window_idx = 0;
            if (ctx->curr_nav_item == 0)
            {
                free_memory_for_switch(ctx);
                ctx->start_view_active = true;
                ctx->start_needs_redraw = true;
            }
            else if (ctx->curr_nav_item == 1) {
                free_memory_for_switch(ctx);
                // ctx->course_view_active = true;
                // ctx->course_needs_redraw = true;
                ctx->active_window = ctx->course_windows[0];
                go_to_course_by_id(ctx, ctx->current_course_id);
            }
            else if (ctx->curr_nav_item == 2) {
                free_memory_for_switch(ctx);
                ctx->active_window = ctx->all_courses_windows[0];
                ctx->all_courses_view_active = true;
                ctx->all_courses_needs_redraw = true;
            }
            else if (ctx->curr_nav_item == 3) {
                free_memory_for_switch(ctx);
                ctx->active_window = ctx->progress_windows[0];
                ctx->progress_view_active = true;
                ctx->progress_needs_redraw = true;
            }
            else if (ctx->curr_nav_item == 4) {
                free_memory_for_switch(ctx);
                ctx->active_window = ctx->keybindings_windows[0];
                ctx->keybindings_view_active = true;
                ctx->keybindings_needs_redraw = true;
            }
            break;
    }
}
