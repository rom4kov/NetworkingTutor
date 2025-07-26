#define _XOPEN_SOURCE_EXTENDED 1
#define _GNU_SOURCE

#include "../../core/core.h"
#include "../../models/models.h"
#include "../start/start_menu.h"
#include "../views.h"
#include "../../course_tests/tests.h"
#include <curses.h>
#include <dirent.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define WINDOW_COUNT 4
#define WU COLS / 12 // WU for WIDTH_UNIT
#define WIDTH_REMAINDER COLS % WU
#define EXPLORER_WIDTH WU + WU / 2
#define EDITOR_WIDTH ((WU * 7 + 4) - (WU + WU / 2))
#define EDIT_WIN_WIDTH WU * 5 + WU / 2

void create_course_view(APP_CONTEXT *ctx)
{
    ctx->active_window = 0;

    ctx->course_windows[0] =
        create_navigation_window(&ctx->active_window, &ctx->start_menu);
    ctx->course_windows[1] = create_explorer_window(ctx->file_tree);
    ctx->course_windows[2] = create_editor_window();
    ctx->course_windows[3] =
        create_right_side_panel(ctx, " Course instructions ");
    ctx->course_windows[4] = create_progress_window(ctx);

    ctx->line_num_win = derwin(ctx->course_windows[2], LINES - 7, 3, 2, 1);
    ctx->edit_window =
        derwin(ctx->course_windows[2], LINES - 7, WU * 5 + (WU / 2) - 2, 2, 5);

    if (ctx->file == NULL)
    {
        wattron(ctx->line_num_win, COLOR_PAIR(11));
        for (int i = 0; i < LINES - 4; ++i)
        {
            mvwprintw(ctx->line_num_win, i, 1, "%s", i % 2 == 0 ? "`" : " ");
        }
        wattroff(ctx->line_num_win, COLOR_PAIR(11));
        wattron(ctx->course_windows[2], COLOR_PAIR(10));
        print_no_open_file_msg(&ctx->course_windows[2]);
        wattroff(ctx->course_windows[2], COLOR_PAIR(10));
    }

    wnoutrefresh(ctx->course_windows[2]);
    wnoutrefresh(ctx->course_windows[4]);

    initialize_testing(ctx);
}

WINDOW *create_editor_window()
{
    WINDOW *editor_window = newwin(LINES - 3, EDITOR_WIDTH, 3, WU + WU / 2);
    draw_border(editor_window, 2, "Editor");

    // mvwprintw(editor_window, 0, 2, "%i", *active_window);

    wattron(editor_window, COLOR_PAIR(3));
    mvwprintw(editor_window, 0, 2, " Editor ");
    wattroff(editor_window, COLOR_PAIR(3));
    // mvwprintw(editor_window, 9, 2, "COLS: %i", COLS);
    // mvwprintw(editor_window, 10, 2, "Editor width: %i", EDITOR_WIDTH);

    wnoutrefresh(editor_window);

    return editor_window;
}

WINDOW *create_explorer_window(FILE_TREE *file_tree)
{
    WINDOW *explorer_window = newwin(LINES - 3, EXPLORER_WIDTH, 3, 0);
    draw_border(explorer_window, 2, " Explorer ");

    wattron(explorer_window, COLOR_PAIR(3));
    mvwprintw(explorer_window, 0, 2, " Explorer ");
    wattroff(explorer_window, COLOR_PAIR(3));

    create_explorer_menu(&explorer_window, file_tree);

    return explorer_window;
}

void create_explorer_menu(WINDOW **explorer_window, FILE_TREE *f_tree)
{
    DIR *dir = opendir(".");

    struct dirent *next = readdir(dir);

    DIR_ENTRY *prev_dir = initialize_dir_entry();
    DIR_ENTRY *curr_dir = initialize_dir_entry();

    if (f_tree->num_of_entries == 0)
    {
        while (strcmp(next->d_name, ".") == 0 ||
               strcmp(next->d_name, "..") == 0 || next->d_type != 4)
        {
            next = readdir(dir);
        }
        strncpy(prev_dir->name, next->d_name, 30);
        prev_dir->name[29] = '\0';
        // mvwprintw(*explorer_window, LINES - 7, 2, "%s", prev_dir->name);
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
                curr_dir = initialize_dir_entry();
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
                curr_dir->prev = prev_dir;
                prev_dir->next = curr_dir;
                prev_dir = curr_dir;
                curr_dir = initialize_dir_entry();
                f_tree->num_of_entries++;
                f_tree->current_entry = f_tree->current_entry->next;
            }
            next = readdir(dir);
        }

        f_tree->current_entry = f_tree->first_entry;
        rewinddir(dir);
    }

    mvwprintw(*explorer_window, LINES - 5, 2, "Press ? for Keys");

    f_tree->current_entry = f_tree->first_entry;

    print_entries(f_tree, explorer_window);

    // select current entry of file tree
    f_tree->current_entry = f_tree->first_entry;
    for (int i = 0; i < f_tree->curr_entry_nr && f_tree->current_entry != NULL;
         i++)
    {
        f_tree->current_entry = f_tree->current_entry->next;
    }
    if (f_tree->curr_entry_nr == 0)
    {
        f_tree->current_entry = f_tree->first_entry;
    }

    wnoutrefresh(*explorer_window);
    closedir(dir);
}

WINDOW *create_progress_window(APP_CONTEXT *ctx)
{
    WINDOW *progress_window =
        newwin(3, ctx->rp_state->window_width, LINES - 3, WU * 7 + 4);
    draw_border(progress_window, 2, " Progress ");

    wattron(progress_window, COLOR_PAIR(3));
    mvwprintw(progress_window, 0, 2, " Status ");
    wattroff(progress_window, COLOR_PAIR(3));

    mvwprintw(progress_window, 1, 2, "Course: %s", ctx->current_course);
    // char *progress_text = "Progress: Courses 0 -- Sections 0";
    // mvwprintw(progress_window, 1,
    //           ctx->rp_state->window_width - strlen(progress_text) - 2, "%s",
    //           progress_text);
    mvwprintw(progress_window, 1, ctx->rp_state->window_width - 23,
              "Sections completed: %i", ctx->rp_state->sections_completed);
    wnoutrefresh(progress_window);
    return progress_window;
}

void open_sub_directory(char *dir_name, FILE_TREE *f_tree)
{
    DIR *dir = opendir(dir_name);

    struct dirent *next = readdir(dir);

    DIR_ENTRY *prev_dir = f_tree->current_entry;
    DIR_ENTRY *curr_dir = initialize_dir_entry();

    DIR_ENTRY *next_orig_dir = f_tree->current_entry->next;

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
            curr_dir->parent_dir = f_tree->current_entry;

            curr_dir->prev = prev_dir;
            prev_dir->next = curr_dir;
            prev_dir = curr_dir;
            curr_dir = initialize_dir_entry();
            f_tree->current_entry->num_of_entries++;
            f_tree->num_of_entries++;
        }
        next = readdir(dir);
    }
    rewinddir(dir);
    next = readdir(dir);
    while (NULL != next)
    {
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
            curr_dir->parent_dir = f_tree->current_entry;

            curr_dir->prev = prev_dir;
            prev_dir->next = curr_dir;
            prev_dir = curr_dir;
            curr_dir = initialize_dir_entry();
            f_tree->current_entry->num_of_entries++;
            f_tree->num_of_entries++;
        }
        next = readdir(dir);
    }

    prev_dir->next = next_orig_dir;
    if (f_tree->current_entry->num_of_entries > 0)
        prev_dir->last_in_sub_dir = true;
    next_orig_dir->prev = prev_dir;

    DIR_ENTRY *entries_iterator = f_tree->current_entry;

    while (entries_iterator->parent_dir)
    {
        entries_iterator->parent_dir->num_of_entries +=
            f_tree->current_entry->num_of_entries;
        entries_iterator = entries_iterator->parent_dir;
    }

    closedir(dir);
}

void close_sub_directory(DIR_ENTRY *dir_to_close, int entries_in_dir,
                         FILE_TREE *f_tree)
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

    curr_dir->next = curr_entry;
    if (curr_entry)
        curr_entry->prev = curr_dir;
    f_tree->num_of_entries -= entries_in_dir;

    DIR_ENTRY *entries_iterator = f_tree->current_entry;

    while (entries_iterator->parent_dir)
    {
        entries_iterator->parent_dir->num_of_entries -=
            dir_to_close->num_of_entries;
        entries_iterator = entries_iterator->parent_dir;
    }

    // if (dir_to_close->parent_dir != NULL)
    //     dir_to_close->parent_dir->num_of_entries -=
    //         dir_to_close->num_of_entries;
    dir_to_close->num_of_entries -= entries_in_dir;
}

void print_no_open_file_msg(WINDOW **editor_window)
{
    char *msg1 = "No file has been opened yet.";
    char *msg2 = "Open or create a file in the file explorer";
    int left_pad1 = (EDITOR_WIDTH - strlen(msg1)) / 2;
    int left_pad2 = (EDITOR_WIDTH - strlen(msg2)) / 2;
    mvwprintw(*editor_window, LINES - (LINES / 2) - 6, left_pad1, "%s", msg1);
    mvwprintw(*editor_window, LINES - (LINES / 2) - 5, left_pad2, "%s", msg2);
}
