#include "../data/data_access_layer.h"
#include "../models/models.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>

void handle_explorer_input(int ch, TEXT_BUFFER *tbuf, FILE *file,
                           WINDOW **explorer_win, WINDOW **line_num_win,
                           WINDOW **editor_window, WINDOW **edit_window,
                           bool *explorer_mode, MENU **explorer_menu)
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
        case 10:
            curr_item = current_item(*explorer_menu);
            const char *name = item_name(curr_item);
            deallocate_buffer(tbuf);
            tbuf = initialize_buffer();
            file =
                open_file(name, tbuf, line_num_win, editor_window, edit_window);
            rewind(file);
            // mvwprintw(*explorer_win, LINES - 23, 2, "                  ");
            // mvwprintw(*explorer_win, LINES - 23, 2, "%s", name);
            wnoutrefresh(*explorer_win);
            wnoutrefresh(*line_num_win);
            wnoutrefresh(*editor_window);
            wnoutrefresh(*edit_window);
            doupdate();
            break;
        case KEY_F(1):
            wrefresh(*explorer_win);
            *explorer_mode = false;
            break;
    }
}
