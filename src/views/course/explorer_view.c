#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <ncurses.h>
#include <string.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void print_entries(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    int items = 0;

    while (f_tree->current_entry != NULL)
    {
        if (strcmp(f_tree->current_entry->name, "..") != 0 &&
            strcmp(f_tree->current_entry->name, ".") != 0)
        {
            if (f_tree->current_entry->type == 4)
            {
                DIR_ENTRY *parent_level = initialize_dir_entry();
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                {
                    parent_level = f_tree->current_entry; 
                    for (int k = j + 1;
                         k < f_tree->current_entry->indent_level; k++)
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
                mvwprintw(
                    *explorer_window, items + 1,
                    4 + f_tree->current_entry->indent_level * 2, "%s",
                    trunc_str(f_tree->current_entry->name, (WU + WU / 2) - 2,
                              4 + f_tree->current_entry->indent_level * 2));
                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);

                items++;
            }
            else if (f_tree->current_entry->type != 4)
            {
                DIR_ENTRY *parent_level = initialize_dir_entry();
                wattron(*explorer_window, COLOR_PAIR(10));
                for (int j = 0; j < f_tree->current_entry->indent_level; j++)
                {
                    parent_level = f_tree->current_entry; 
                    for (int k = j + 1;
                         k < f_tree->current_entry->indent_level; k++)
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
                mvwprintw(
                    *explorer_window, items + 1,
                    4 + f_tree->current_entry->indent_level * 2, "%s",
                    trunc_str(f_tree->current_entry->name, (WU + WU / 2) - 2,
                              4 + f_tree->current_entry->indent_level * 2));
                if (f_tree->curr_entry_nr == items)
                    wattroff(*explorer_window, A_BOLD | A_ITALIC);

                items++;
            }
        }
        f_tree->current_entry = f_tree->current_entry->next;
    }
}

void move_to_next_entry(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    f_tree->current_entry = f_tree->current_entry->next;
    f_tree->curr_entry_nr++;
    *explorer_window = create_explorer_window(f_tree);
    focus_window(explorer_window, 3, "Explorer");
    wnoutrefresh(*explorer_window);
    doupdate();
}

void move_to_prev_entry(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    f_tree->current_entry = f_tree->current_entry->prev;
    f_tree->curr_entry_nr--;
    *explorer_window = create_explorer_window(f_tree);
    focus_window(&*explorer_window, 3, "Explorer");
    wnoutrefresh(*explorer_window);
    doupdate();
}
