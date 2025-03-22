#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

#define WINDOW_COUNT 4
#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDIT_MAX WU * 7 + 4

void handle_course_input(int *ch, WINDOW **windows, WINDOW **line_num_win,
                         WINDOW **edit_window, bool *start_view_active,
                         bool *course_view_active, int *active_win,
                         bool *start_needs_redraw, MENU **start_menu,
                         ITEM **curr_item, MENU **explorer_menu,
                         ITEM ***menu_items, char **filename,
                         bool *explorer_mode, bool *editor_mode, FILE **file,
                         TEXT_BUFFER *t_buffer, int *scroll_offset,
                         int *lines_to_print, int *y, int *x)
{
    if (*active_win == 0)
    {
        switch (*ch)
        {
            case KEY_LEFT:
                menu_driver(*start_menu, REQ_PREV_ITEM);
                wrefresh(windows[0]);
                break;
            case KEY_RIGHT:
                menu_driver(*start_menu, REQ_NEXT_ITEM);
                wrefresh(windows[0]);
                break;
            case 9:
            case KEY_DOWN:
                *active_win = 2;
                focus_window(&windows[0], 2, "Navigation");
                focus_window(&windows[2], 3, "Editor");
                break;
            case '\n':
                *curr_item = current_item(*start_menu);
                if (item_index(*curr_item) == 0)
                {
                    *course_view_active = false;
                    *start_view_active = true;
                    for (int i = 0; i < WINDOW_COUNT; ++i)
                    {
                        wclear(windows[i]);
                    }
                    *course_view_active = false;
                    *start_view_active = true;
                    *start_needs_redraw = true;
                    break;
                }
                else if (item_index(*curr_item) == 2)
                {
                    focus_window(&windows[0], 2, "Navigation");
                    focus_window(&windows[5], 3, "Details");
                    *active_win = 5;
                    focus_window(&windows[5], 3, "Details");
                    wmove(windows[5], 4, 14);
                    wrefresh(windows[5]);
                }
                break;
        }
    }
    else if (*active_win == 1 && *explorer_mode)
    {
        handle_explorer_input(
            *ch, t_buffer, file, filename, &windows[1], line_num_win,
            &windows[2], edit_window, editor_mode, explorer_mode, explorer_menu,
            menu_items, scroll_offset, lines_to_print, active_win);
    }
    else if (*active_win == 1)
    {
        switch (*ch)
        {
            case 9:
            case KEY_RIGHT:
                *active_win = 2;
                focus_window(&windows[1], 2, "Explorer");
                focus_window(&windows[2], 3, "Editor");
                break;
            case KEY_UP:
                *active_win = 0;
                focus_window(&windows[1], 2, "Explorer");
                focus_window(&windows[0], 3, "Navigation");
                break;
            case 10:
                *explorer_mode = true;
                break;
        }
    }
    else if (*active_win == 2 && *editor_mode && *file)
    {
        handle_editor_input(*ch, line_num_win, edit_window, t_buffer, *file,
                            editor_mode, scroll_offset, lines_to_print, y, x);
    }
    else if (*active_win == 2)
    {
        switch (*ch)
        {
            case 9:
            case KEY_LEFT:
                *active_win = 1;
                focus_window(&windows[2], 2, "Editor");
                focus_window(&windows[1], 3, "Explorer");
                break;
            case KEY_RIGHT:
                *active_win = 3;
                focus_window(&windows[2], 2, "Editor");
                focus_window(&windows[3], 3, "Course Instructions");
                break;
            case KEY_UP:
                *active_win = 0;
                focus_window(&windows[0], 3, "Navigation");
                focus_window(&windows[2], 2, "Editor");
                break;
            case 10:
                *editor_mode = true;
                wmove(*edit_window, 0, 0);
                wrefresh(*edit_window);
                break;
        }
    }
    else if (*active_win == 3)
    {
        switch (*ch)
        {
            case 9:
            case KEY_LEFT:
                *active_win = 2;
                focus_window(&windows[2], 3, "Editor");
                focus_window(&windows[3], 2, "Course Instructions");
                break;
        }
    }
}
