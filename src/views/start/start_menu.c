// #include "start_menu.h"
// #include <menu.h>
// #include <ncurses.h>
// #include <stdlib.h>
//
// #define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
// #define CTRLD 4
//
// char *start_menu_strings[] = {
//     "Home",        "Browse courses",     "Account",    "Your Progress",
//     "Preferences", "Keyboard shortcuts", (char *)NULL,
// };
//
// MENU *create_start_menu(WINDOW **start_menu_win)
// {
//     ITEM **start_menu_items;
//     MENU *start_menu;
//     int n_items, i;
//
//     n_items = ARRAY_SIZE(start_menu_strings);
//     start_menu_items = (ITEM **)calloc(n_items + 1, sizeof(ITEM *));
//
//     for (i = 0; i < n_items; i++)
//     {
//         start_menu_items[i] = new_item(start_menu_strings[i], start_menu_strings[i]);
//     }
//     start_menu_items[n_items] = (ITEM *)NULL;
//
//     start_menu = new_menu((ITEM **)start_menu_items);
//
//     // WINDOW *sub_win = derwin(*start_menu_win, n_items + 1, COLS / 6 - 2, 1, 1);
//
//     set_menu_win(start_menu, *start_menu_win);
//     // set_menu_sub(start_menu, sub_win);
//
//     mvwprintw(*start_menu_win, 1, 1, "hello");
//     post_menu(start_menu);
//     mvwprintw(*start_menu_win, 2, 1, "%d", n_items);
//
//     wrefresh(*start_menu_win);
//     // refresh();
//     // wrefresh(sub_win);
//
//     // for (int i = 0; i < n_items; i++)
//     // {
//     //     free_item(start_menu_items[i]);
//     // }
//     // free_menu(start_menu);
//     return start_menu;
// }
