#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <ncurses.h>
#include <curses.h>
#include <string.h>
// #include <sys/types.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void print_entries(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    int items = 0;
    // int last_entry = 0;
    while (f_tree->current_entry != NULL)
    {
        if (strcmp(f_tree->current_entry->name, "..") != 0 &&
            strcmp(f_tree->current_entry->name, ".") != 0)
        {
            if (f_tree->current_entry->type == 4)
            {
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                    mvwaddch(*explorer_window, items + 1, j + 2, ACS_VLINE);
                // mvwprintw(*explorer_window, 30, 2, "%i", last_entry);
                // if (f_tree->current_entry->indent_level > 0 &&
                //     f_tree->current_entry->entry_nr == last_entry)
                //     mvwaddch(*explorer_window, items + 1, 2, ACS_LLCORNER);

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
                          trunc_str(f_tree->current_entry->name,
                                    (WU + WU / 2) - 2,
                                    4 + f_tree->current_entry->indent_level));
                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);

                items++;

                // if (f_tree->current_entry->state == 'o')
                // {
                //     print_sub_entries(f_tree, &items, explorer_window);
                // }

            }
            else if (f_tree->current_entry->type != 4)
            {
                // (*menu_items)[items] =
                //     new_item(f_tree->current_entry->name, "");

                // mvwprintw(*win, i + 2, 2, " "); mvwprintw(*win, i + 2, 2,
                // "%s", return_trimmed(f_tree->current_entry->name));
                // wrefresh(*win);
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                    mvwaddch(*explorer_window, items + 1, j + 2, ACS_VLINE);
                // if (f_tree->current_entry->indent_level > 0)
                // {
                //     mvwaddch(*explorer_window, items + 1, 2, ACS_LLCORNER);
                //     mvwprintw(*explorer_window, 32, 2, "%i", last_entry);
                //     mvwprintw(*explorer_window, 33, 2, "%i",
                //               f_tree->current_entry->indent_level);
                //     mvwprintw(*explorer_window, 34, 2, "%i",
                //               f_tree->current_entry->entry_nr);
                // }
                wattroff(*explorer_window, COLOR_PAIR(10));
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
                          trunc_str(f_tree->current_entry->name, (WU + WU / 2) - 2,
                                    4 + f_tree->current_entry->indent_level));
                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);
                items++;
            }
        }
        f_tree->current_entry = f_tree->current_entry->next;
    }
    //
    // f_tree->current_entry = f_tree->first_entry;
    // for (int i = 0; i < f_tree->curr_entry_nr; i++)
    // {
    //     f_tree->current_entry = f_tree->current_entry->next;
    // }
    // if (f_tree->curr_entry_nr == 0)
    // {
    //     f_tree->current_entry = f_tree->first_entry;
    // }
}

void print_sub_entries(FILE_TREE *f_tree, int *items, WINDOW **explorer_window)
{
    int last_entry = f_tree->current_entry->num_of_entries - 1;
    f_tree->current_entry = f_tree->current_entry->next;

    for (int i = 0; i < f_tree->current_entry->prev->num_of_entries; i++)
    {
        if (strcmp(f_tree->current_entry->name, "..") != 0 &&
            strcmp(f_tree->current_entry->name, ".") != 0)
        {
            if (f_tree->current_entry->type == 4)
            {
                // if (f_tree->current_entry->num_of_entries > 0)
                    // last_entry = f_tree->current_entry->entry_nr +
                    //              f_tree->current_entry->num_of_entries;
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                    mvwaddch(*explorer_window, *items + 1, j + 2, ACS_VLINE);
                // mvwprintw(*explorer_window, 30, 2, "%i", last_entry);
                if (f_tree->current_entry->sub_entry_nr == last_entry)
                    mvwaddch(*explorer_window, *items + 1, 2, ACS_LLCORNER);
                if (f_tree->current_entry->state == 'c')
                {
                    mvwprintw(*explorer_window, *items + 1,
                              2 + f_tree->current_entry->indent_level, " ");
                }
                else
                {
                    mvwprintw(*explorer_window, *items + 1,
                              2 + f_tree->current_entry->indent_level, " ");
                }
                wattroff(*explorer_window, COLOR_PAIR(10));
                if (f_tree->curr_entry_nr == *items + 1)
                    wattron(*explorer_window, A_BOLD | A_ITALIC);
                mvwprintw(*explorer_window, *items + 1,
                          4 + f_tree->current_entry->indent_level, "%s",
                          trunc_str(f_tree->current_entry->name,
                                    (WU + WU / 2) - 2,
                                    4 + f_tree->current_entry->indent_level));
                if (f_tree->curr_entry_nr == *items + 1)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);

                *items += 1;
            }
            else if (f_tree->current_entry->type != 4)
            {
                // (*menu_*items + 1)[*items + 1] =
                //     new_item(f_tree->current_entry->name, "");

                // mvwprintw(*win, i + 2, 2, " "); mvwprintw(*win, i + 2, 2,
                // "%s", return_trimmed(f_tree->current_entry->name));
                // wrefresh(*win);
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                    mvwaddch(*explorer_window, *items + 1, j + 2, ACS_VLINE);
                if (f_tree->current_entry->indent_level > 0)
                {
                    mvwaddch(*explorer_window, *items + 1, 2, ACS_LLCORNER);
                    mvwprintw(*explorer_window, 32, 2, "%i", last_entry);
                    mvwprintw(*explorer_window, 33, 2, "%i",
                              f_tree->current_entry->indent_level);
                    mvwprintw(*explorer_window, 34, 2, "%i",
                              f_tree->current_entry->entry_nr);
                }
                wattroff(*explorer_window, COLOR_PAIR(10));
                ICON icon =
                    print_file_icon((char *)f_tree->current_entry->name);
                if (icon.icon != NULL)
                {
                    wattron(*explorer_window, COLOR_PAIR(icon.color));
                    mvwprintw(*explorer_window, *items + 1,
                              2 + f_tree->current_entry->indent_level, "%s",
                              icon.icon);
                    wattroff(*explorer_window, COLOR_PAIR(icon.color));
                }
                else
                {
                    mvwprintw(*explorer_window, *items + 1,
                              2 + f_tree->current_entry->indent_level, " ");
                }

                if (f_tree->curr_entry_nr == *items + 1)
                    wattron(*explorer_window, A_BOLD | A_ITALIC);
                mvwprintw(*explorer_window, *items + 1,
                          4 + f_tree->current_entry->indent_level, "%s",
                          f_tree->current_entry->name,
                          (WU + WU / 2) - 2,
                          4 + f_tree->current_entry->indent_level);
                if (f_tree->curr_entry_nr == *items + 1)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);
                *items += 1;
            }
        }
        f_tree->current_entry = f_tree->current_entry->next;
    }
}
