#define _XOPEN_SOURCE_EXTENDED 1

#include "../core/core.h"
#include "start_menu.h"
#include "views.h"
#include "views.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>

#define WINDOW_COUNT 3
#define WU COLS / 12 // WU for WIDTH_UNIT
#define WIDTH_REMAINDER COLS % WU

void create_course_view(sqlite3 *db)
{
    WINDOW *windows[WINDOW_COUNT];
    int active_window = 0;
    MENU *start_menu;

    windows[0] = create_navigation_window(&active_window, &start_menu);
    windows[1] = create_editor_window(&active_window);
    windows[2] = create_right_side_panel(&active_window, &db, "Course instructions");

    wrefresh(windows[2]);

    handle_course_input(windows, &active_window, &start_menu, db);
}

WINDOW *create_editor_window(int *active_window)
{
    WINDOW *editor_window = newwin(LINES - 3, WU * 7 + 4, 3, 0);
    draw_border(editor_window, 2, 0);

    mvwprintw(editor_window, 0, 2, "%i", *active_window);

    wattron(editor_window, COLOR_PAIR(3));
    mvwprintw(editor_window, 0, 2, "Editor");
    wattroff(editor_window, COLOR_PAIR(3));

    wrefresh(editor_window);

    return editor_window;
}

void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win)
{
    wclear(*line_num_win);
    wclear(*edit_window);
    int i;

    LINE *next_line = tbuf->first_line;

    wattron(*line_num_win, COLOR_PAIR(4));
    for (i = 0; i < tbuf->num_of_lines; i++)
    {
        if (i < 9)
        {
            mvwprintw(*line_num_win, i, 1, "%i", i + 1);
        }
        else
        {
            mvwprintw(*line_num_win, i, 0, "%i", i + 1);
        }
        mvwprintw(*edit_window, i, 0, "%s", next_line->buf_);
        next_line = next_line->next;
    }
    wattroff(*line_num_win, COLOR_PAIR(4));
}
