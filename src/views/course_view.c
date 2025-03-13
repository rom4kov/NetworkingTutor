#define _XOPEN_SOURCE_EXTENDED 1
#define _GNU_SOURCE

#include "../core/core.h"
#include "start_menu.h"
#include "views.h"
#include <curses.h>
#include <dirent.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <stdio.h>
#include <sys/types.h>

#define WINDOW_COUNT 4
#define WU COLS / 12 // WU for WIDTH_UNIT
#define WIDTH_REMAINDER COLS % WU

void create_course_view(sqlite3 *db)
{
    WINDOW *windows[WINDOW_COUNT];
    int active_window = 0;
    MENU *start_menu;

    windows[0] = create_navigation_window(&active_window, &start_menu);
    windows[1] = create_explorer_window(&active_window);
    windows[2] = create_editor_window(&active_window);
    windows[3] =
        create_right_side_panel(&active_window, &db, "Course instructions");

    wrefresh(windows[2]);

    handle_course_input(windows, &active_window, &start_menu, db);
}

WINDOW *create_editor_window(int *active_window)
{
    WINDOW *editor_window =
        newwin(LINES - 3, WU * 5 + WU / 2 + 5, 3, WU + WU / 2);
    draw_border(editor_window, 2, "Editor");

    mvwprintw(editor_window, 0, 2, "%i", *active_window);

    wattron(editor_window, COLOR_PAIR(3));
    mvwprintw(editor_window, 0, 2, "Editor");
    wattroff(editor_window, COLOR_PAIR(3));

    wrefresh(editor_window);

    return editor_window;
}

WINDOW *create_explorer_window(int *active_window)
{
    WINDOW *explorer_window = newwin(LINES - 3, WU + WU / 2, 3, 0);
    draw_border(explorer_window, 2, "Explorer");

    mvwprintw(explorer_window, 0, 2, "%i", *active_window);

    wattron(explorer_window, COLOR_PAIR(3));
    mvwprintw(explorer_window, 0, 2, "Explorer");
    wattroff(explorer_window, COLOR_PAIR(3));

    int i = 1;
    DIR *dir = opendir(".");
    struct dirent *next = readdir(dir);

    while (NULL != next)
    {
        mvwprintw(explorer_window, i, 2, "%s", next->d_name);
        next = readdir(dir);
        i++;
    }

    wrefresh(explorer_window);

    return explorer_window;
}
