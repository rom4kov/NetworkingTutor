#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_explorer_popup_windows(APP_CONTEXT *ctx)
{
    ctx->explorer_popup_windows[0] =
        derwin(ctx->course_windows[1], 3, EXPLORER_WIDTH - 2, 1, 1);
    ctx->explorer_popup_windows[1] =
        derwin(ctx->explorer_popup_windows[0], 1, EXPLORER_WIDTH - 4, 1, 1);
    ctx->explorer_popup_windows[2] =
        derwin(ctx->course_windows[1], 3, EXPLORER_WIDTH - 2, 1, 1);
    ctx->explorer_popup_windows[3] =
        derwin(ctx->explorer_popup_windows[2], 1, EXPLORER_WIDTH - 4, 1, 1);
    ctx->explorer_popup_windows[4] =
        derwin(ctx->course_windows[1], 3, EXPLORER_WIDTH - 2, 1, 1);
    ctx->explorer_popup_windows[5] =
        derwin(ctx->explorer_popup_windows[4], 1, EXPLORER_WIDTH - 4, 1, 1);
    ctx->explorer_popup_windows[6] =
        derwin(ctx->course_windows[1], 3, EXPLORER_WIDTH - 2, 1, 1);
    ctx->explorer_popup_windows[7] =
        derwin(ctx->explorer_popup_windows[6], 1, EXPLORER_WIDTH - 4, 1, 1);
}

void create_explorer_panels(APP_CONTEXT *ctx)
{
    ctx->explorer_panels[0] = new_panel(ctx->course_windows[1]);
    ctx->explorer_panels[1] = new_panel(ctx->explorer_popup_windows[0]);
    ctx->explorer_panels[2] = new_panel(ctx->explorer_popup_windows[2]);
    ctx->explorer_panels[3] = new_panel(ctx->explorer_popup_windows[4]);
    ctx->explorer_panels[4] = new_panel(ctx->explorer_popup_windows[6]);

    ctx->explorer_panels[5] = new_panel(ctx->course_windows[0]);
    ctx->explorer_panels[6] = new_panel(ctx->course_windows[2]);
    ctx->explorer_panels[7] = new_panel(ctx->course_windows[3]);
    ctx->explorer_panels[8] = new_panel(ctx->course_windows[4]);
}

void print_entries(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    int items = 0;
    DIR_ENTRY *parent_level = NULL;

    while (f_tree->current_entry != NULL)
    {
        if (strcmp(f_tree->current_entry->name, "..") != 0 &&
            strcmp(f_tree->current_entry->name, ".") != 0)
        {
            if (f_tree->current_entry->type == 4)
            {
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                {
                    parent_level = f_tree->current_entry;
                    for (int k = j + 1; k < f_tree->current_entry->indent_level;
                         k++)
                        parent_level = parent_level->parent_dir;
                    if (parent_level && !parent_level->last_in_sub_dir)
                        mvwaddch(*explorer_window, items + 1, (j * 2) + 2,
                                 ACS_VLINE);
                }

                if (f_tree->current_entry->last_in_sub_dir)
                    mvwaddch(*explorer_window, items + 1,
                             f_tree->current_entry->indent_level * 2,
                             ACS_LLCORNER);
                if (f_tree->current_entry->state == 'c')
                {
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level * 2,
                              " ");
                }
                else
                {
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level * 2,
                              " ");
                }
                wattroff(*explorer_window, COLOR_PAIR(10));

                if (f_tree->curr_entry_nr == items)
                    wattron(*explorer_window, A_BOLD | A_ITALIC);

                char *truncated_str =
                    trunc_str(f_tree->current_entry->name, (WU + WU / 2) - 2,
                              4 + f_tree->current_entry->indent_level * 2);
                mvwprintw(*explorer_window, items + 1,
                          4 + f_tree->current_entry->indent_level * 2, "%s",
                          truncated_str);
                free(truncated_str);

                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);

                items++;
            }
            else if (f_tree->current_entry->type != 4)
            {
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                {
                    parent_level = f_tree->current_entry;
                    for (int k = j + 1; k < f_tree->current_entry->indent_level;
                         k++)
                        parent_level = parent_level->parent_dir;
                    if (parent_level && !parent_level->last_in_sub_dir)
                        mvwaddch(*explorer_window, items + 1, (j * 2) + 2,
                                 ACS_VLINE);
                }
                if (f_tree->current_entry->last_in_sub_dir)
                    mvwaddch(*explorer_window, items + 1,
                             f_tree->current_entry->indent_level * 2,
                             ACS_LLCORNER);
                wattroff(*explorer_window, COLOR_PAIR(10));

                ICON icon = get_file_icon((char *)f_tree->current_entry->name);
                if (icon.icon != NULL)
                {
                    wattron(*explorer_window, COLOR_PAIR(icon.color));
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level * 2, "%s",
                              icon.icon);
                    wattroff(*explorer_window, COLOR_PAIR(icon.color));
                }
                else
                {
                    mvwprintw(*explorer_window, items + 1,
                              2 + f_tree->current_entry->indent_level * 2,
                              " ");
                }

                if (f_tree->curr_entry_nr == items)
                    wattron(*explorer_window, A_BOLD | A_ITALIC);

                char *truncated_str =
                    trunc_str(f_tree->current_entry->name, (WU + WU / 2) - 2,
                              4 + f_tree->current_entry->indent_level * 2);
                mvwprintw(*explorer_window, items + 1,
                          4 + f_tree->current_entry->indent_level * 2, "%s",
                          truncated_str);
                free(truncated_str);

                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);

                items++;
            }
        }
        f_tree->current_entry = f_tree->current_entry->next;
    }

    wattron(*explorer_window, COLOR_PAIR(11));
    for (int i = items; i < LINES - 4; i++)
    {
        mvwprintw(*explorer_window, i, 1, "%s", i % 2 == 0 ? "`" : " ");
    }
    wattroff(*explorer_window, COLOR_PAIR(11));
}

void move_to_next_entry(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    f_tree->curr_entry_nr++;
    f_tree->current_entry = f_tree->current_entry->next;
    werase(*explorer_window);
    create_file_tree(explorer_window, f_tree);
    focus_window(explorer_window, 3, "Explorer");
    doupdate();
}

void move_to_prev_entry(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    f_tree->curr_entry_nr--;
    f_tree->current_entry = f_tree->current_entry->prev;
    werase(*explorer_window);
    create_file_tree(explorer_window, f_tree);
    focus_window(explorer_window, 3, "Explorer");
    doupdate();
}
