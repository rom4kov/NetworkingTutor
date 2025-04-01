#include <unistd.h>
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
    ctx->course_windows[1] =
        create_explorer_window(ctx->file_tree, &ctx->course_windows[2], 0);
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

WINDOW *create_explorer_window(FILE_TREE *file_tree, WINDOW **win, int i_idx)
{
    WINDOW *explorer_window = newwin(LINES - 3, WU + WU / 2, 3, 0);
    draw_border(explorer_window, 2, "Explorer");

    wattron(explorer_window, COLOR_PAIR(3));
    mvwprintw(explorer_window, 0, 2, "Explorer");
    wattroff(explorer_window, COLOR_PAIR(3));

    create_explorer_menu(&explorer_window, file_tree, win, i_idx);

    return explorer_window;
}

void create_explorer_menu(WINDOW **explorer_window, FILE_TREE *f_tree,
                          WINDOW **win, int i_idx)
{
    DIR *dir = opendir(".");

    struct dirent *next = readdir(dir);

    DIR_ENTRY *prev_dir = initialize_dir_entry(explorer_window, 0);
    DIR_ENTRY *curr_dir = initialize_dir_entry(explorer_window, 0);

    if (f_tree->num_of_entries == 0)
    {
        while (strcmp(next->d_name, ".") == 0 ||
               strcmp(next->d_name, "..") == 0 || next->d_type != 4)
        {
            next = readdir(dir);
        }
        strncpy(prev_dir->name, next->d_name, 30);
        prev_dir->name[29] = '\0';
        mvwprintw(*explorer_window, LINES - 7, 2, "%s", prev_dir->name);
        strncpy(prev_dir->path, next->d_name, 30);
        prev_dir->path[29] = '\0';
        prev_dir->type = next->d_type;
        prev_dir->state = 'c';
        prev_dir->prev = NULL;
        f_tree->current_entry = curr_dir;
        prev_dir->next = f_tree->current_entry;
        f_tree->current_entry->prev = prev_dir;
        f_tree->first_entry = prev_dir;
        f_tree->num_of_entries++;
        rewinddir(dir);
        // next = readdir(dir);

        while (NULL != next)
        {
            if (next->d_type == 4 && strcmp(next->d_name, "..") != 0 &&
                strcmp(next->d_name, ".") != 0 &&
                strcmp(next->d_name, f_tree->first_entry->name) != 0)
            {
                strncpy(curr_dir->name, next->d_name, 30);
                curr_dir->name[29] = '\0';
                strncpy(curr_dir->path, next->d_name, 30);
                curr_dir->path[29] = '\0';
                curr_dir->type = next->d_type;
                curr_dir->state = 'c';
                curr_dir->prev = prev_dir;
                prev_dir->next = curr_dir;
                prev_dir = curr_dir;
                curr_dir = initialize_dir_entry(
                    win, f_tree->current_entry->num_of_entries);
                f_tree->num_of_entries++;
            }
            next = readdir(dir);
        }
        rewinddir(dir);
        next = readdir(dir);
        while (NULL != next)
        {
            if (next->d_type != 4)
            {
                strncpy(curr_dir->name, next->d_name, 30);
                curr_dir->name[29] = '\0';
                strncpy(curr_dir->path, next->d_name, 30);
                curr_dir->path[29] = '\0';
                curr_dir->type = next->d_type;
                curr_dir->state = 'c';
                // curr_dir->indent_level = 0;
                curr_dir->prev = prev_dir;
                prev_dir->next = curr_dir;
                prev_dir = curr_dir;
                curr_dir = initialize_dir_entry(
                    win, f_tree->current_entry->num_of_entries);
                f_tree->num_of_entries++;
                f_tree->current_entry = f_tree->current_entry->next;
            }
            next = readdir(dir);
        }

        f_tree->current_entry = f_tree->first_entry;
        rewinddir(dir);
    }

    f_tree->current_entry = f_tree->first_entry;

    mvwprintw(*explorer_window, LINES - 5, 2, "Press ? for Keys");
    mvwprintw(*explorer_window, LINES - 6, 2, "%s", f_tree->first_entry->name);

    // unpost_menu(*explorer_menu);
    // free_menu(*explorer_menu);
    // if (*explorer_menu != NULL)
    // {
    //     for (int i = 0; (*menu_items)[i] != NULL; i++)
    //     {
    //         free_item((*menu_items)[i]);
    //     }
    // }

    // *menu_items = (ITEM **)calloc(f_tree->num_of_entries + 1, sizeof(ITEM
    // *));
    // (*menu_items)[f_tree->num_of_entries] = NULL;

    int items = 0;
    // for (int i = 0; i < f_tree->num_of_entries; i++)
    // {
    //     if (strcmp(f_tree->current_entry->name, "..") != 0 &&
    //         strcmp(f_tree->current_entry->name, ".") != 0)
    //     {
    //         (*menu_items)[items] = new_item(f_tree->current_entry->name, "");
    //         items++;
    //     }
    //     f_tree->current_entry = f_tree->current_entry->next;
    // }

    f_tree->current_entry = f_tree->first_entry;

    // items = 0;
    for (int i = 0; i < f_tree->num_of_entries; i++)
    {
        if (strcmp(f_tree->current_entry->name, "..") != 0 &&
            strcmp(f_tree->current_entry->name, ".") != 0)
        {
            // int len = strlen(f_tree->current_entry->name);
            // int ind_level = f_tree->current_entry->indent_level;
            //
            // memmove(&f_tree->current_entry->name[ind_level],
            //         f_tree->current_entry->name, len);
            // memset(f_tree->current_entry->name, ' ', ind_level);

            if (f_tree->current_entry->type == 4)
            {
                wattron(*explorer_window, COLOR_PAIR(10));
                if (f_tree->current_entry->state == 'c')
                {
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level, " ");
                }
                else
                {
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level, " ");
                }
                wattroff(*explorer_window, COLOR_PAIR(10));
                if (f_tree->curr_entry_nr == items)
                    wattron(*explorer_window, A_BOLD | A_ITALIC);
                mvwprintw(*explorer_window, items + 1,
                          4 + f_tree->current_entry->indent_level, "%s",
                          f_tree->current_entry->name);
                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);

                items++;
            }
            else if (f_tree->current_entry->type != 4)
            {
                // (*menu_items)[items] =
                //     new_item(f_tree->current_entry->name, "");

                // mvwprintw(*win, i + 2, 2, " "); mvwprintw(*win, i + 2, 2,
                // "%s", return_trimmed(f_tree->current_entry->name));
                // wrefresh(*win);
                ICON icon =
                    print_file_icon((char *)f_tree->current_entry->name);
                if (icon.icon != NULL)
                {
                    wattron(*explorer_window, COLOR_PAIR(icon.color));
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level, "%s",
                              icon.icon);
                    wattroff(*explorer_window, COLOR_PAIR(icon.color));
                }
                else
                {
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level, " ");
                }

                if (f_tree->curr_entry_nr == items)
                    wattron(*explorer_window, A_BOLD | A_ITALIC);
                mvwprintw(*explorer_window, items + 1,
                          4 + f_tree->current_entry->indent_level, "%s",
                          f_tree->current_entry->name);
                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);
                items++;
            }
        }
        f_tree->current_entry = f_tree->current_entry->next;
    }

    f_tree->current_entry = f_tree->first_entry;
    for (int i = 0; i < f_tree->curr_entry_nr; i++)
    {
        f_tree->current_entry = f_tree->current_entry->next;
    }
    if (f_tree->curr_entry_nr == 0)
    {
        f_tree->current_entry = f_tree->first_entry;
    }
    // f_tree->current_entry = f_tree->first_entry;
    // *explorer_menu = new_menu(*menu_items);
    // set_menu_format(*explorer_menu, 30, 1);
    // set_menu_spacing(*explorer_menu, 0, 1, 0);
    //
    // set_menu_win(*explorer_menu, *explorer_window);
    // set_menu_sub(*explorer_menu,
    //              derwin(*explorer_window, LINES - 7, WU + (WU / 2) - 6, 1,
    //              4));
    // set_menu_fore(*explorer_menu, A_BOLD | A_ITALIC | COLOR_PAIR(2));
    // set_menu_back(*explorer_menu, COLOR_PAIR(1));
    // set_menu_mark(*explorer_menu, "");
    //
    // post_menu(*explorer_menu);

    // for (int i = 0; i < i_idx; i++)
    // {
    //     menu_driver(*explorer_menu, REQ_NEXT_ITEM);
    // }

    wrefresh(*explorer_window);
    // closedir(dir);
}

void open_sub_directory(char *dir_name, FILE_TREE *f_tree, WINDOW **win)
{
    DIR *dir = opendir(dir_name);

    struct dirent *next = readdir(dir);

    DIR_ENTRY *prev_dir = f_tree->current_entry;
    DIR_ENTRY *curr_dir =
        initialize_dir_entry(win, f_tree->current_entry->num_of_entries);

    DIR_ENTRY *next_orig_dir = f_tree->current_entry->next;
    // int len = strlen(f_tree->current_entry->path);

    while (NULL != next)
    {
        if (next->d_type == 4 && strcmp(next->d_name, "..") != 0 &&
            strcmp(next->d_name, ".") != 0)
        {
            strncpy(curr_dir->name, next->d_name, 30);
            curr_dir->name[29] = '\0';

            strncpy(curr_dir->path, dir_name, strlen(dir_name));
            memset(&curr_dir->path[strlen(dir_name)], '/', 1);
            memmove(&curr_dir->path[strlen(dir_name) + 1], curr_dir->name,
                    strlen(curr_dir->name));
            memset(
                &curr_dir->path[strlen(dir_name) + 1 + strlen(curr_dir->name)],
                '\0', 1);

            curr_dir->type = next->d_type;
            curr_dir->state = 'c';
            curr_dir->indent_level = f_tree->current_entry->indent_level + 1;

            // int len = strlen(curr_dir->name);
            // int ind_level = curr_dir->indent_level;
            //
            // memmove(&curr_dir->name[ind_level], curr_dir->name, len);
            // memset(curr_dir->name, ' ', ind_level);
            // mvwprintw(*win, 30, 2, "%i", f_tree->current_entry->indent_level);
            // wrefresh(*win);

            curr_dir->prev = prev_dir;
            prev_dir->next = curr_dir;
            prev_dir = curr_dir;
            curr_dir = initialize_dir_entry(
                win, f_tree->current_entry->num_of_entries);
            f_tree->current_entry->num_of_entries++;
            f_tree->num_of_entries++;
        }
        next = readdir(dir);
    }
    rewinddir(dir);
    next = readdir(dir);
    while (NULL != next)
    {
        strncpy(curr_dir->name, next->d_name, 30);
        if (next->d_type == 8)
        {
            strncpy(curr_dir->name, next->d_name, 30);
            curr_dir->name[29] = '\0';

            strncpy(curr_dir->path, dir_name, strlen(dir_name));
            memset(&curr_dir->path[strlen(dir_name)], '/', 1);
            memmove(&curr_dir->path[strlen(dir_name) + 1], curr_dir->name,
                    strlen(curr_dir->name));
            memset(
                &curr_dir->path[strlen(dir_name) + 1 + strlen(curr_dir->name)],
                '\0', 1);

            curr_dir->type = next->d_type;
            curr_dir->state = 'c';
            curr_dir->indent_level = f_tree->current_entry->indent_level + 1;

            // int len = strlen(curr_dir->name);
            // int ind_level = curr_dir->indent_level;
            // mvwprintw(*win, 32, 2, "%i", f_tree->current_entry->indent_level);
            // wrefresh(*win);
            //
            // memmove(&curr_dir->name[ind_level], curr_dir->name, len);
            // memset(curr_dir->name, ' ', ind_level);

            curr_dir->prev = prev_dir;
            prev_dir->next = curr_dir;
            prev_dir = curr_dir;
            curr_dir = initialize_dir_entry(
                win, f_tree->current_entry->num_of_entries);
            f_tree->current_entry->num_of_entries++;
            f_tree->num_of_entries++;
        }
        next = readdir(dir);
    }

    prev_dir->next = next_orig_dir;
    next_orig_dir->prev = prev_dir;

    // closedir(dir);
}

void close_sub_directory(DIR_ENTRY *dir_to_close, int entries_in_dir,
                         FILE_TREE *f_tree, WINDOW **win)
{
    DIR_ENTRY *curr_dir = dir_to_close;
    DIR_ENTRY *curr_entry = curr_dir->next;
    DIR_ENTRY *next_entry = NULL;

    for (int i = 0; i < entries_in_dir; i++)
    {
        if (curr_entry == NULL)
            break;

        next_entry = curr_entry->next;

        if (curr_entry->name != NULL)
        {
            free(curr_entry->name);
            curr_entry->name = NULL;
        }

        if (curr_entry->path != NULL)
        {
            free(curr_entry->path);
            curr_entry->path = NULL;
        }

        free(curr_entry);

        curr_entry = next_entry;
    }
    // mvwprintw(*win, 40, 2, "%s", dir_to_close->name);
    // wrefresh(*win);

    curr_dir->next = curr_entry;
    if (curr_entry)
        curr_entry->prev = curr_dir;
    f_tree->current_entry->num_of_entries -= entries_in_dir;
    f_tree->num_of_entries -= entries_in_dir;
}
