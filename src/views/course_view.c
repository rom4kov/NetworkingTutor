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

void create_course_view(APP_CONTEXT *ctx)
{
    ctx->active_window = 0;

    ctx->course_windows[0] =
        create_navigation_window(&ctx->active_window, &ctx->start_menu);
    ctx->course_windows[1] = create_explorer_window(
        &ctx->explorer_menu, &ctx->menu_items, ctx->file_tree);
    ctx->course_windows[2] = create_editor_window(&ctx->active_window);
    ctx->course_windows[3] = create_right_side_panel(
        &ctx->active_window, &ctx->db, "Course instructions");

    ctx->line_num_win = derwin(ctx->course_windows[2], LINES - 6, 3, 2, 1);
    ctx->edit_window =
        derwin(ctx->course_windows[2], LINES - 6, WU * 5 + (WU / 2) - 2, 2, 5);

    wattron(ctx->course_windows[2], COLOR_PAIR(10));
    mvwprintw(ctx->course_windows[2], LINES - (LINES / 2) - 4, 7,
              "No file has been opened yet. Open or create a file in the file "
              "explorer");
    wattroff(ctx->course_windows[2], COLOR_PAIR(10));
    wrefresh(ctx->course_windows[2]);
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

WINDOW *create_explorer_window(MENU **explorer_menu, ITEM ***menu_items,
                               FILE_TREE *file_tree)
{
    WINDOW *explorer_window = newwin(LINES - 3, WU + WU / 2, 3, 0);
    draw_border(explorer_window, 2, "Explorer");

    wattron(explorer_window, COLOR_PAIR(3));
    mvwprintw(explorer_window, 0, 2, "Explorer");
    wattroff(explorer_window, COLOR_PAIR(3));

    create_explorer_menu(&explorer_window, explorer_menu, menu_items,
                         file_tree);

    return explorer_window;
}

void create_explorer_menu(WINDOW **explorer_window, MENU **explorer_menu,
                          ITEM ***menu_items, FILE_TREE *f_tree)
{

    if (f_tree->num_of_entries == 0)
    {
        DIR *dir = opendir(".");

        DIR_ENTRY *prev_dir = initialize_dir_entry();
        DIR_ENTRY *curr_dir = initialize_dir_entry();

        struct dirent *next = readdir(dir);

        f_tree->first_entry->name = next->d_name;
        f_tree->first_entry->type = next->d_type;
        f_tree->first_entry->state = 'c';
        f_tree->first_entry->prev = NULL;
        f_tree->current_entry = curr_dir;
        f_tree->first_entry->next = f_tree->current_entry;
        f_tree->current_entry->prev = f_tree->first_entry;
        f_tree->num_of_entries++;
        prev_dir = f_tree->first_entry;
        next = readdir(dir);

        while (NULL != next)
        {
            curr_dir->name = next->d_name;
            curr_dir->type = next->d_type;
            curr_dir->state = 'c';
            curr_dir->prev = prev_dir;
            prev_dir->next = curr_dir;
            prev_dir = curr_dir;
            curr_dir = initialize_dir_entry();
            f_tree->num_of_entries++;
            next = readdir(dir);
        }

        f_tree->current_entry = f_tree->first_entry;
        rewinddir(dir);
    }

    // mvwprintw(*explorer_window, 31, 2, "%i", f_tree->num_of_entries);
    // mvwprintw(*explorer_window, 32, 2, "%i", f_tree->current_entry->type);
    // mvwprintw(*explorer_window, 33, 2, "%s", f_tree->current_entry->name);
    // mvwprintw(*explorer_window, 34, 2, "%s",
    // f_tree->current_entry->next->name); mvwprintw(*explorer_window, 35, 2,
    // "%i", f_tree->current_entry->next->type); mvwprintw(*explorer_window, 36,
    // 2, "%s", f_tree->current_entry->next->next->name);
    mvwprintw(*explorer_window, LINES - 5, 2, "Press ? for Keys");

    unpost_menu(*explorer_menu);
    free_menu(*explorer_menu);
    if (*explorer_menu != NULL)
    {
        for (int i = 0; (*menu_items)[i] != NULL; i++)
        {
            free_item((*menu_items)[i]);
        }
    }

    *menu_items = (ITEM **)calloc(f_tree->num_of_entries, sizeof(ITEM *));
    (*menu_items)[f_tree->num_of_entries - 1] = NULL;

    int items = 0;
    for (int i = 0; i < f_tree->num_of_entries - 1; i++)
    {
        // next = readdir(dir);
        // mvwprintw(*explorer_window, i + 15, 2, "%i",
        // f_tree->current_entry->type); mvwprintw(*explorer_window, i + 15, 4,
        // "%s", f_tree->current_entry->name);
        if (strcmp(f_tree->current_entry->name, "..") != 0 &&
            strcmp(f_tree->current_entry->name, ".") != 0)
        {
            if (f_tree->current_entry->type == 4)
            {
                // entries[items].name = next->d_name;
                // entries[items].type = next->d_type;
                // if (entries[items].state == 'o')
                // {
                //     open_sub_directory(next->d_name, &dir_size, &items,
                //     *menu_items, entries);
                // }
                // mvwprintw(*explorer_window, i + 25, 2, "%i",
                // f_tree->current_entry->type); mvwprintw(*explorer_window, 32,
                // 2, "%i", f_tree->num_of_entries);
                wattron(*explorer_window, COLOR_PAIR(10));
                mvwprintw(*explorer_window, items + 1, 2, " ");
                wattroff(*explorer_window, COLOR_PAIR(10));

                (*menu_items)[items] =
                    new_item(f_tree->current_entry->name, "");

                // ICON icon = print_file_icon((char
                // *)f_tree->current_entry->name); if (icon.icon != NULL)
                // {
                //     wattron(*explorer_window, COLOR_PAIR(icon.color));
                //     mvwprintw(*explorer_window, items + 1, 2, "%s",
                //     icon.icon); wattroff(*explorer_window,
                //     COLOR_PAIR(icon.color));
                // }
                items++;
            }
        }
        f_tree->current_entry = f_tree->current_entry->next;
    }

    // rewinddir(dir);
    f_tree->current_entry = f_tree->first_entry;

    for (int i = 0; i < f_tree->num_of_entries - 1; i++)
    {
        // next = readdir(dir);
        if (strcmp(f_tree->current_entry->name, "..") != 0 &&
            strcmp(f_tree->current_entry->name, ".") != 0)
        {
            if (f_tree->current_entry->type != 4)
            {
                // entries[items].name = next->d_name;
                // entries[items].type = next->d_type;

                (*menu_items)[items] =
                    new_item(f_tree->current_entry->name, "");

                ICON icon =
                    print_file_icon((char *)f_tree->current_entry->name);
                if (icon.icon != NULL)
                {
                    wattron(*explorer_window, COLOR_PAIR(icon.color));
                    mvwprintw(*explorer_window, items + 1, 2, "%s", icon.icon);
                    wattroff(*explorer_window, COLOR_PAIR(icon.color));
                }
                items++;
            }
        }
        f_tree->current_entry = f_tree->current_entry->next;
    }

    f_tree->current_entry = f_tree->first_entry;

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

void open_sub_directory(char *dir_name, int *dir_size, int *items,
                        ITEM **menu_items, DIR_ENTRY *entries)
{
    DIR *dir = opendir(dir_name);

    // int dir_size = 0;

    struct dirent *next = readdir(dir);

    while (NULL != next)
    {
        dir_size++;
        next = readdir(dir);
    }
}
