#include "../core/core.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>

#define WINDOW_COUNT 3
#define WU COLS / 12 // WU for WIDTH_UNIT

void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu, sqlite3 *db)
{
    bool in_course_view = true;
    bool editor_mode = false;
    int ch;
    ITEM *curr_item;
    int y, x;
    y = 0;
    x = 0;

    WINDOW *edit_window = derwin(windows[1], LINES - 5, WU * 7 + 2, 1, 1);
    // FIELD *fields[3];

    while (in_course_view)
    {
        ch = getch();
        ESCDELAY = 100;

        if (ch == 27)
            return;

        if (*active_win == 0)
        {
            switch (ch)
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
                    *active_win = 1;
                    focus_window(windows, 0, 2, active_win, "Navigation");
                    focus_window(windows, 1, 3, active_win, "Editor");
                    break;
                case '\n':
                    curr_item = current_item(*start_menu);
                    if (item_index(curr_item) == 0)
                    {
                        in_course_view = false;
                        for (int i = 0; i < WINDOW_COUNT; ++i)
                        {
                            // delwin(windows[i]);
                            wclear(windows[i]);
                        }
                        create_start_screen(db);
                        break;
                    }
                    else if (item_index(curr_item) == 2)
                    {
                        focus_window(windows, 0, 2, active_win, "Navigation");
                        focus_window(windows, 5, 3, active_win, "Details");
                        *active_win = 5;
                        mvwprintw(windows[5], 30, 2, "active window: %i",
                                  *active_win);
                        // create_user_form(&windows[5], &user_form, fields);
                        focus_window(windows, 5, 3, active_win, "Details");
                        wmove(windows[5], 4, 14);
                        wrefresh(windows[5]);
                    }
                    break;
            }
        }
        else if (*active_win == 1 && editor_mode)
        {
            switch (ch) {
                case KEY_RIGHT:
                    getyx(edit_window, y, x);
                    wmove(edit_window, y, x + 1);
                    wrefresh(edit_window);
                    break;
                case KEY_LEFT:
                    getyx(edit_window, y, x);
                    wmove(edit_window, y, x - 1);
                    wrefresh(edit_window);
                    break;
                case KEY_DOWN:
                    getyx(edit_window, y, x);
                    wmove(edit_window, y + 1, x);
                    wrefresh(edit_window);
                    break;
                case KEY_UP:
                    getyx(edit_window, y, x);
                    wmove(edit_window, y - 1, x);
                    wrefresh(edit_window);
                    break;
                case KEY_BACKSPACE:
                    getyx(edit_window, y, x);
                    if (x == 0)
                    {
                        wmove(edit_window, y - 1, x);
                        wrefresh(edit_window);
                        break;
                    }
                    mvwprintw(edit_window, y, x - 1, " ");
                    wmove(edit_window, y, x - 1);
                    wrefresh(edit_window);
                    break;
                case 10:
                    getyx(edit_window, y, x);
                    wprintw(edit_window, "%c", '\n');
                    wmove(edit_window, y + 1, 0);
                    wrefresh(edit_window);
                    break;
                case KEY_F(1):
                    curs_set(0);
                    wrefresh(edit_window);
                    editor_mode = false;
                    break;
                default:
                    wprintw(edit_window, "%c", ch);
                    // wmove(edit_window, y, x + 1);
                    wrefresh(edit_window);
                    break;
            }
        }
        else if (*active_win == 1)
        {
            switch (ch)
            {
                case 9:
                case KEY_RIGHT:
                    *active_win = 2;
                    focus_window(windows, 1, 2, active_win, "Editor");
                    focus_window(windows, 2, 3, active_win, "Course Instructions");
                    break;
                case KEY_UP:
                    *active_win = 0;
                    focus_window(windows, 0, 3, active_win, "Navigation");
                    focus_window(windows, 1, 2, active_win, "Editor");
                    break;
                case 10:
                    editor_mode = true;
                    curs_set(1);
                    wmove(windows[1], 1, 1);
                    wrefresh(windows[1]);
                    break;
            }
        }
        else if (*active_win == 2)
        {
            switch (ch)
            {
                case 9:
                case KEY_LEFT:
                    *active_win = 1;
                    focus_window(windows, 1, 3, active_win, "Editor");
                    focus_window(windows, 2, 2, active_win, "Course Instructions");
                    break;
            }
        }
    }
}
