#define _XOPEN_SOURCE_EXTENDED 1
#define _GNU_SOURCE

#include "../core/core.h"
#include "../models/models.h"
#include "start_menu.h"
#include "views.h"
#include <curses.h>
#include <dirent.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define WINDOW_COUNT 4
#define WU COLS / 12 // WU for WIDTH_UNIT
#define WIDTH_REMAINDER COLS % WU

void create_course_view(WINDOW **windows, WINDOW **line_num_win,
                        WINDOW **edit_window, int *active_window,
                        MENU **start_menu, MENU **explorer_menu,
                        ITEM ***menu_items, sqlite3 *db)
{
    *active_window = 0;

    windows[0] = create_navigation_window(active_window, start_menu);
    windows[1] = create_explorer_window(explorer_menu, menu_items);
    windows[2] = create_editor_window(active_window);
    windows[3] =
        create_right_side_panel(active_window, &db, "Course instructions");

    *line_num_win = derwin(windows[2], LINES - 6, 3, 2, 1);
    *edit_window =
        derwin(windows[2], LINES - 6, WU * 5 + (WU / 2) - 1, 2, 4);

    wattron(windows[2], COLOR_PAIR(10));
    mvwprintw(windows[2], LINES - (LINES / 2) - 4, 7,
              "No file has been opened yet. Open or create a file in the file "
              "explorer");
    wattroff(windows[2], COLOR_PAIR(10));
    wrefresh(windows[2]);
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

WINDOW *create_explorer_window(MENU **explorer_menu, ITEM ***menu_items)
{
    WINDOW *explorer_window = newwin(LINES - 3, WU + WU / 2, 3, 0);
    draw_border(explorer_window, 2, "Explorer");

    wattron(explorer_window, COLOR_PAIR(3));
    mvwprintw(explorer_window, 0, 2, "Explorer");
    wattroff(explorer_window, COLOR_PAIR(3));

    create_explorer_menu(&explorer_window, explorer_menu, menu_items);

    return explorer_window;
}

void create_explorer_menu(WINDOW **explorer_window, MENU **explorer_menu,
                          ITEM ***menu_items)
{
    DIR *dir = opendir(".");

    int dir_size = 0;

    struct dirent *next = readdir(dir);

    while (NULL != next)
    {
        dir_size++;
        next = readdir(dir);
    }

    mvwprintw(*explorer_window, LINES - 5, 2, "Press ? for Keys");

    rewinddir(dir);

    unpost_menu(*explorer_menu);
    free_menu(*explorer_menu);
    if (*explorer_menu != NULL)
    {
        for (int i = 0; (*menu_items)[i] != NULL; i++)
        {
            free_item((*menu_items)[i]);
        }
    }

    *menu_items = (ITEM **)calloc(dir_size, sizeof(ITEM *));
    (*menu_items)[dir_size - 1] = NULL;

    int item = 0;
    for (int i = 0; i < dir_size - 1; i++)
    {
        next = readdir(dir);
        if (strcmp(next->d_name, "..") != 0 && strcmp(next->d_name, ".") != 0)
        {
            if (next->d_type == 4)
            {
                wattron(*explorer_window, COLOR_PAIR(10));
                mvwprintw(*explorer_window, item + 1, 2, " ");
                wattroff(*explorer_window, COLOR_PAIR(10));
                (*menu_items)[item] = new_item(next->d_name, "");
                ICON icon = print_file_icon((char *)next->d_name);
                if (icon.icon != NULL)
                {
                    wattron(*explorer_window, COLOR_PAIR(icon.color));
                    mvwprintw(*explorer_window, item + 1, 2, "%s", icon.icon);
                    wattroff(*explorer_window, COLOR_PAIR(icon.color));
                }
                item++;
            }
        }
    }

    rewinddir(dir);

    for (int i = 0; i < dir_size - 1; i++)
    {
        next = readdir(dir);
        if (strcmp(next->d_name, "..") != 0 && strcmp(next->d_name, ".") != 0)
        {
            if (next->d_type != 4)
            {
                (*menu_items)[item] = new_item(next->d_name, "");
                ICON icon = print_file_icon((char *)next->d_name);
                if (icon.icon != NULL)
                {
                    wattron(*explorer_window, COLOR_PAIR(icon.color));
                    mvwprintw(*explorer_window, item + 1, 2, "%s", icon.icon);
                    wattroff(*explorer_window, COLOR_PAIR(icon.color));
                }
                item++;
            }
        }
    }

    *explorer_menu = new_menu(*menu_items);
    set_menu_format(*explorer_menu, 30, 1);
    set_menu_spacing(*explorer_menu, 0, 1, 0);

    set_menu_win(*explorer_menu, *explorer_window);
    set_menu_sub(*explorer_menu,
                 derwin(*explorer_window, LINES - 7, WU + (WU / 2) - 6, 1, 4));
    set_menu_fore(*explorer_menu, A_BOLD | A_ITALIC | COLOR_PAIR(2));
    set_menu_back(*explorer_menu, COLOR_PAIR(1));
    set_menu_mark(*explorer_menu, "");

    post_menu(*explorer_menu);

    wrefresh(*explorer_window);
}
