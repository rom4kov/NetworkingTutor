#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

#define WINDOW_COUNT 3
#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDIT_MAX WU * 7 + 4

void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu,
                         MENU **explorer_menu, sqlite3 *db)
{
    bool in_course_view = true;
    bool editor_mode = false;
    bool explorer_mode = false;
    int ch;
    ITEM *curr_item;

    WINDOW *line_num_win = derwin(windows[2], LINES - 6, 3, 2, 1);
    WINDOW *edit_window =
        derwin(windows[2], LINES - 6, WU * 5 + (WU / 2) - 2, 2, 4);

    const char *filename = "../read_lines.c";

    TEXT_BUFFER *t_buffer = initialize_buffer();

    FILE *file =
        open_file(filename, t_buffer, &line_num_win, &windows[2], &edit_window);

    wrefresh(line_num_win);
    wrefresh(windows[2]);

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
                    *active_win = 2;
                    focus_window(windows, 0, 2, "Navigation");
                    focus_window(windows, 2, 3, "Editor");
                    break;
                case '\n':
                    curr_item = current_item(*start_menu);
                    if (item_index(curr_item) == 0)
                    {
                        in_course_view = false;
                        for (int i = 0; i < WINDOW_COUNT; ++i)
                        {
                            wclear(windows[i]);
                        }
                        create_start_screen(db);
                        break;
                    }
                    else if (item_index(curr_item) == 2)
                    {
                        focus_window(windows, 0, 2, "Navigation");
                        focus_window(windows, 5, 3, "Details");
                        *active_win = 5;
                        focus_window(windows, 5, 3, "Details");
                        wmove(windows[5], 4, 14);
                        wrefresh(windows[5]);
                    }
                    break;
            }
        }
        else if (*active_win == 1 && explorer_mode)
        {
            handle_explorer_input(ch, t_buffer, file, &windows[1],
                                  &line_num_win, &windows[2], &edit_window,
                                  &explorer_mode, explorer_menu);
        }
        else if (*active_win == 1)
        {
            switch (ch)
            {
                case 9:
                case KEY_RIGHT:
                    *active_win = 2;
                    focus_window(windows, 1, 2, "Explorer");
                    focus_window(windows, 2, 3, "Editor");
                    break;
                case KEY_UP:
                    *active_win = 0;
                    focus_window(windows, 1, 2, "Explorer");
                    focus_window(windows, 0, 3, "Navigation");
                    break;
                case 10:
                    explorer_mode = true;
                    // wrefresh(windows[1]);
                    break;
            }
        }
        else if (*active_win == 2 && editor_mode)
        {
            handle_editor_input(ch, &line_num_win, &edit_window, t_buffer, file,
                                &editor_mode);
        }
        else if (*active_win == 2)
        {
            switch (ch)
            {
                case 9:
                case KEY_LEFT:
                    *active_win = 1;
                    focus_window(windows, 2, 2, "Editor");
                    focus_window(windows, 1, 3, "Explorer");
                    break;
                case KEY_RIGHT:
                    *active_win = 3;
                    focus_window(windows, 2, 2, "Editor");
                    focus_window(windows, 3, 3, "Course Instructions");
                    break;
                case KEY_UP:
                    *active_win = 0;
                    focus_window(windows, 0, 3, "Navigation");
                    focus_window(windows, 2, 2, "Editor");
                    break;
                case 10:
                    editor_mode = true;
                    curs_set(1);
                    wmove(edit_window, 0, 0);
                    wrefresh(edit_window);
                    break;
            }
        }
        else if (*active_win == 3)
        {
            switch (ch)
            {
                case 9:
                case KEY_LEFT:
                    *active_win = 2;
                    focus_window(windows, 2, 3, "Editor");
                    focus_window(windows, 3, 2, "Course Instructions");
                    break;
            }
        }
    }
    fclose(file);
}
