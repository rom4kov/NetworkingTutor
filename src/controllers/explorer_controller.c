#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

void handle_explorer_input(int ch, TEXT_BUFFER *tbuf, FILE *file,
                           WINDOW **explorer_win, WINDOW **line_num_win,
                           WINDOW **editor_window, WINDOW **edit_window,
                           bool *editor_mode, bool *explorer_mode,
                           MENU **explorer_menu, ITEM ***menu_items,
                           int *scroll_offset, int *lines_to_print,
                           int *active_window)
{
    ITEM *curr_item;
    bool new_file_form_active = false;
    WINDOW *inner_win = derwin(*explorer_win, 3, 18, 2, 2);
    WINDOW *form_window = derwin(inner_win, 1, 16, 1, 1);
    FORM *new_file_form = NULL;
    FIELD *field[2];

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
            fclose(file);
            file = open_file(name, tbuf, line_num_win, editor_window,
                             edit_window, scroll_offset, lines_to_print);
            rewind(file);

            new_file_form_active = false;
            *explorer_mode = false;
            *editor_mode = true;
            *active_window = 2;
            focus_window(explorer_win, 2, "Explorer");
            focus_window(editor_window, 3, "Editor");
            curs_set(2);
            wmove(*edit_window, 0, 0);
            wnoutrefresh(*explorer_win);
            wnoutrefresh(*line_num_win);
            wnoutrefresh(*editor_window);
            wnoutrefresh(*edit_window);
            doupdate();
            break;
        case 'a':
            create_new_file_input(&inner_win, &form_window, &new_file_form,
                                  field);

            new_file_form_active = true;

            while (new_file_form_active)
            {
                ch = getch();

                switch (ch)
                {
                    case 263: // Backspace
                        form_driver(new_file_form, REQ_VALIDATION);
                        FIELD *current = current_field(new_file_form);
                        char *buf = field_buffer(current, 0);
                        trim(buf);
                        if (buf && get_length(buf) > 0)
                        {
                            // form_driver(new_file_form, REQ_LEFT_CHAR);
                            form_driver(new_file_form, REQ_DEL_PREV);
                            wrefresh(form_window);
                        }
                        break;
                    case 10:
                        form_driver(new_file_form, REQ_VALIDATION);
                        char *new_file_name = field_buffer(field[0], 0);
                        trim(new_file_name);
                        new_file_form_active = false;
                        *explorer_mode = false;
                        *editor_mode = true;
                        *active_window = 2;
                        focus_window(editor_window, 3, "Editor");

                        deallocate_buffer(tbuf);
                        tbuf = initialize_buffer();
                        if (file)
                            fclose(file);
                        file = open_new_file(new_file_name, tbuf, line_num_win,
                                             editor_window, edit_window,
                                             scroll_offset, lines_to_print);

                        *explorer_win =
                            create_explorer_window(explorer_menu, menu_items);
                        wmove(*edit_window, 0, 0);
                        wrefresh(*explorer_win);
                        wrefresh(*edit_window);
                        break;
                    case 'q':
                        new_file_form_active = false;
                        curs_set(0);
                        unpost_form(new_file_form);
                        free_form(new_file_form);
                        free_field(field[0]);
                        menu_driver(*explorer_menu, REQ_NEXT_ITEM);
                        *explorer_win =
                            create_explorer_window(explorer_menu, menu_items);
                        break;
                    default:
                        form_driver(new_file_form, ch);
                        wrefresh(form_window);
                        break;
                }
            }

            break;
        case KEY_F(1):
            wrefresh(*explorer_win);
            *explorer_mode = false;
            break;
    }
}
