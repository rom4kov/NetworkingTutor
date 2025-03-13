#include <ncurses.h>
#include <menu.h>

void handle_explorer_input(int ch, WINDOW **explorer_win, bool *explorer_mode,
                           MENU **explorer_menu)
{
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
        case KEY_F(1):
            *explorer_mode = false;
            break;
    }
}
