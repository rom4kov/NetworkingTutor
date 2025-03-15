#define _XOPEN_SOURCE 500

#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "controllers.h"
#include "utils.h"
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         TEXT_BUFFER *text_buf, FILE *file, bool *editor_mode,
                         int *scroll_offset)
{
    int y, x;
    getyx(*edit_window, y, x);

    int lines_to_print =
        text_buf->num_of_lines > LINES - 7 ? LINES - 7 : text_buf->num_of_lines;

    switch (ch)
    {
        case KEY_RIGHT:
            move_right(text_buf, edit_window, y, x);
            break;
        case KEY_LEFT:
            move_left(text_buf, edit_window, y, x);
            break;
        case KEY_DOWN:
            move_down(text_buf, line_num_win, edit_window, y, x, scroll_offset, lines_to_print);
            break;
        case KEY_UP:
            move_up(text_buf, line_num_win, edit_window, y, x, scroll_offset, lines_to_print);
            break;
        case KEY_BACKSPACE:
            bs_delete_char_or_line(text_buf, line_num_win, edit_window, y, x,
                                   scroll_offset, lines_to_print);
            break;
        case KEY_DC:
            delete_char_or_line(text_buf, line_num_win, edit_window, y, x,
                                scroll_offset, lines_to_print);
            break;
        case 9:
            insert_tab(text_buf, edit_window, y, x);
            break;
        case 10:
            insert_line(text_buf, edit_window, line_num_win, y, x,
                        scroll_offset, lines_to_print);
            break;
        case KEY_HOME:
            move_to_start_of_line(text_buf, edit_window, y, x);
            break;
        case KEY_END:
            move_to_end_of_line(text_buf, edit_window, y, x);
            break;
        case KEY_F(1):
            curs_set(0);
            wrefresh(*edit_window);
            *editor_mode = false;
            break;
        case KEY_F(2):
            break;
        case KEY_F(10):
            write_buffer_to_file(text_buf, file, y);
            break;
        default:
            insert_char(text_buf, edit_window, y, x, ch);
            break;
    }
}
