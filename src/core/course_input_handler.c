#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_COUNT 3
#define WU COLS / 12 // WU for WIDTH_UNIT

void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu,
                         sqlite3 *db)
{
    bool in_course_view = true;
    bool editor_mode = false;
    int ch;
    ITEM *curr_item;

    WINDOW *line_num_win = derwin(windows[1], LINES - 6, 3, 2, 1);
    WINDOW *edit_window = derwin(windows[1], LINES - 6, WU * 7 - 2, 2, 4);

    char *filename = "../hello.c";
    FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        printf("Could not open %s.\n", filename);
    }

    TEXT_BUFFER *t_buffer = initialize_buffer();
    if (file != NULL)
    {
        read_file_into_buffer(file, t_buffer);

        print_buffer(t_buffer, &edit_window, &line_num_win);

        rewind(file);

        wattron(windows[1], A_BOLD);
        mvwprintw(windows[1], 1, 1, "%s", filename);
        wattroff(windows[1], A_BOLD);
        wrefresh(edit_window);
    }

    wrefresh(line_num_win);
    wrefresh(windows[1]);

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
                        focus_window(windows, 5, 3, active_win, "Details");
                        wmove(windows[5], 4, 14);
                        wrefresh(windows[5]);
                    }
                    break;
            }
        }
        else if (*active_win == 1 && editor_mode)
        {
            handle_editor_input(ch, &line_num_win, &edit_window, t_buffer, file,
                                &editor_mode);
        }
        else if (*active_win == 1)
        {
            switch (ch)
            {
                case 9:
                case KEY_RIGHT:
                    *active_win = 2;
                    focus_window(windows, 1, 2, active_win, "Editor");
                    focus_window(windows, 2, 3, active_win,
                                 "Course Instructions");
                    break;
                case KEY_UP:
                    *active_win = 0;
                    focus_window(windows, 0, 3, active_win, "Navigation");
                    focus_window(windows, 1, 2, active_win, "Editor");
                    break;
                case 10:
                    editor_mode = true;
                    curs_set(1);
                    wmove(edit_window, 0, 0);
                    wrefresh(edit_window);
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
                    focus_window(windows, 2, 2, active_win,
                                 "Course Instructions");
                    break;
            }
        }
    }
    fclose(file);
}
