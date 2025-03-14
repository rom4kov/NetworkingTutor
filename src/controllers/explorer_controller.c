#include <ncurses.h>
#include <menu.h>

void handle_explorer_input(int ch, WINDOW **explorer_win, bool *explorer_mode,
                           MENU **explorer_menu)
{
    ITEM *curr_item;  
    switch (ch)
    {
        case KEY_DOWN:
            menu_driver(*explorer_menu, REQ_NEXT_ITEM);
            wrefresh(*explorer_win);
            break;
        case KEY_UP:
            menu_driver(*explorer_menu, REQ_PREV_ITEM);
            wrefresh(*explorer_win);
            break;
        case 'a':
            curr_item = current_item(*explorer_menu);
            const char *name = item_name(curr_item);
            mvwprintw(*explorer_win, LINES - 23, 2, "                  ");
            mvwprintw(*explorer_win, LINES - 23, 2, "%s", name);
            wrefresh(*explorer_win);
            break;
        case KEY_F(1):
            wrefresh(*explorer_win);
            *explorer_mode = false;
            break;
    }
}
