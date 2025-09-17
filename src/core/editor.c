#include "../../ntutor.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

void move_down(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
               WINDOW **editor_window, int y, int x, int *scroll_offset,
               int lines_to_print, int editor_height)
{
    if (tbuf->curr_line_nr + 1 >= tbuf->num_of_lines)
        return;

    if (tbuf->current_line->next == NULL)
        return;

    if (x + 1 < tbuf->current_line->next->length)
    {
        tbuf->current_line = tbuf->current_line->next;
        tbuf->curr_line_nr++;
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, " ");
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, "%i : %i",
        //           tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr > *scroll_offset + editor_height - 5)
        {
            curs_set(0);
            *scroll_offset += 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         lines_to_print);
            // log_values(edit_window, *scroll_offset, tbuf, lines_to_print, y,
            // x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y, x);
            curs_set(2);
        }
        else
        {
            // log_values(edit_window, *scroll_offset, tbuf, lines_to_print, y,
            // x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y + 1, x);
        }
    }
    else
    {
        tbuf->current_line = tbuf->current_line->next;
        tbuf->curr_line_nr++;
        tbuf->current_col = tbuf->current_line->length - 1;
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, " ");
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, "%i : %i",
        //           tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr > *scroll_offset + editor_height - 5)
        {
            curs_set(0);
            *scroll_offset += 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         lines_to_print);
            // log_values(edit_window, *scroll_offset, tbuf, lines_to_print, y,
            // x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y, tbuf->current_line->length - 1);
            curs_set(2);
        }
        else
        {
            curs_set(0);
            // log_values(edit_window, *scroll_offset, tbuf, lines_to_print, y,
            // x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y + 1, tbuf->current_line->length - 1);
            curs_set(2);
        }
    }
    update_line_numbers(tbuf, line_num_win, scroll_offset, lines_to_print);
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}

void move_up(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
             WINDOW **editor_window, int y, int x, int *scroll_offset,
             int *lines_to_print, int editor_height)
{
    if (tbuf->curr_line_nr == 0)
        return;

    if (y == 0 && *lines_to_print < editor_height - 7 && *scroll_offset > 0)
        *lines_to_print += 1;

    if (x + 1 < tbuf->current_line->prev->length)
    {
        tbuf->current_line = tbuf->current_line->prev;
        tbuf->curr_line_nr--;
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, " ");
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, "%i : %i",
        //           tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr < *scroll_offset)
        {
            curs_set(0);
            *scroll_offset -= 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         *lines_to_print);
            // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y,
            //            x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y, x);
            curs_set(2);
        }
        else
        {
            curs_set(0);
            // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y,
            //            x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y - 1, x);
            curs_set(2);
        }
    }
    else
    {
        tbuf->current_line = tbuf->current_line->prev;
        tbuf->curr_line_nr--;
        tbuf->current_col = tbuf->current_line->length - 1;
        mvwprintw(*edit_window, editor_height - 4, EDITOR_WIDTH - 7,
                  "         ");
        mvwprintw(*edit_window, editor_height - 4, EDITOR_WIDTH - 7, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr < *scroll_offset)
        {
            curs_set(0);
            *scroll_offset -= 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         *lines_to_print);
            // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y,
            //            x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y, tbuf->current_line->length - 1);
            curs_set(2);
        }
        else
        {
            curs_set(0);
            // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y,
            //            x);
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y - 1, tbuf->current_line->length - 1);
            curs_set(2);
        }
    }
    update_line_numbers(tbuf, line_num_win, scroll_offset, *lines_to_print);
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}

void move_right(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
                int y, int x, int editor_height)
{
    if (x + 1 < tbuf->current_line->length)
    {
        tbuf->current_col++;
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, " ");
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, "%i : %i",
        //           tbuf->curr_line_nr, tbuf->current_col);
        print_cursor_position(editor_window, tbuf, editor_height);
        wmove(*edit_window, y, x + 1);
        wrefresh(*edit_window);
    }
}

void move_left(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
               int y, int x, int editor_height)
{
    if (x > 0)
    {
        tbuf->current_col--;
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, " ");
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, "%i : %i",
        //           tbuf->curr_line_nr, tbuf->current_col);
        print_cursor_position(editor_window, tbuf, editor_height);
        wmove(*edit_window, y, x - 1);
        wrefresh(*edit_window);
    }
}

void move_to_start_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                           WINDOW **editor_window, int y, int x,
                           int editor_height)
{
    if (x > 0)
    {
        tbuf->current_col = 0;
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, " ");
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, "%i : %i",
        //           tbuf->curr_line_nr, tbuf->current_col);
        print_cursor_position(editor_window, tbuf, editor_height);
        wmove(*edit_window, y, 0);
        wrefresh(*edit_window);
    }
}

void move_to_end_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                         WINDOW **editor_window, int y, int x,
                         int editor_height)
{
    if (x < tbuf->current_line->length - 1)
    {
        tbuf->current_col = tbuf->current_line->length - 1;
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, " ");
        // mvwprintw(*edit_window, editor_height - 7, EDIT_MAX - 12, "%i : %i",
        //           tbuf->curr_line_nr, tbuf->current_col);
        print_cursor_position(editor_window, tbuf, editor_height);
        wmove(*edit_window, y, tbuf->current_line->length - 1);
        wrefresh(*edit_window);
    }
}

void insert_char(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                 WINDOW **editor_window, int scroll_offset, int y, int x,
                 char ch, WINDOW **line_num_win, int *lines_to_print,
                 int editor_height, int filename_len, bool *file_modified)
{
    memmove(&tbuf->current_line->buf_[x + 1], &tbuf->current_line->buf_[x],
            tbuf->current_line->length - x);
    tbuf->current_line->length++;
    tbuf->current_line->buf_[x] = ch;
    tbuf->current_col++;
    // print_line(tbuf->current_line->buf_, tbuf->curr_line_nr - scroll_offset,
    //            edit_window);
    print_buffer(tbuf, edit_window, line_num_win, &scroll_offset,
                 *lines_to_print);
    // log_values(edit_window, scroll_offset, tbuf, 0, y, x);
    print_cursor_position(editor_window, tbuf, editor_height);
    if (file_modified)
    {
        print_modified_marker(*editor_window, filename_len, file_modified);
    }
    wnoutrefresh(*editor_window);
    wmove(*edit_window, y, x + 1);
    wnoutrefresh(*edit_window);
    doupdate();
}

void insert_tab(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **editor_window,
                int y, int x, WINDOW **line_num_win, int *lines_to_print,
                int *scroll_offset, int editor_height, int filename_len,
                bool *file_modified)
{
    int move_len = 4 - ((x + 4) % 4);
    memmove(&tbuf->current_line->buf_[x + move_len],
            &tbuf->current_line->buf_[x], tbuf->current_line->length - x);
    tbuf->current_line->length += move_len;
    memset(&tbuf->current_line->buf_[x], 32, move_len * sizeof(char));
    tbuf->current_col += move_len;
    // print_line(tbuf->current_line->buf_, tbuf->curr_line_nr, edit_window);
    print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                 *lines_to_print);
    print_cursor_position(editor_window, tbuf, editor_height);
    if (file_modified)
    {
        print_modified_marker(*editor_window, filename_len, file_modified);
    }
    wnoutrefresh(*editor_window);
    wmove(*edit_window, y, x + move_len);
    wnoutrefresh(*edit_window);
    doupdate();
}

void delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                 WINDOW **editor_window, WINDOW **line_num_win, int y, int x,
                 int *scroll_offset, int *lines_to_print, int editor_height,
                 int filename_len, bool *file_modified)
{
    if (tbuf->curr_line_nr + 1 < tbuf->num_of_lines)
    {
        memmove(&tbuf->current_line->buf_[tbuf->current_line->length - 1],
                tbuf->current_line->next->buf_,
                tbuf->current_line->next->length);
        free(tbuf->current_line->next->buf_);
        tbuf->current_line->length += (tbuf->current_line->next->length - 1);
        if (tbuf->curr_line_nr + 2 < tbuf->num_of_lines)
        {
            tbuf->current_line->next->next->prev = tbuf->current_line;
            tbuf->current_line->next = tbuf->current_line->next->next;
        }
        if (tbuf->curr_line_nr < tbuf->num_of_lines - 1)
            if (tbuf->num_of_lines - *scroll_offset < editor_height - 6)
            {
                *lines_to_print -= 1;
            }
        tbuf->num_of_lines--;
        // if (tbuf->num_of_lines - *scroll_offset < editor_height - 7 &&
        //     *scroll_offset + *lines_to_print > tbuf->num_of_lines)
        //     *lines_to_print -= 1;
        curs_set(0);
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y, x);
        print_cursor_position(editor_window, tbuf, editor_height);
        if (file_modified)
        {
            print_modified_marker(*editor_window, filename_len, file_modified);
        }
        wnoutrefresh(*editor_window);
        wmove(*edit_window, y, x);
        curs_set(2);
        wnoutrefresh(*line_num_win);
        wnoutrefresh(*edit_window);
        doupdate();
    }
}

void delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         WINDOW **edit_window, WINDOW **editor_window, int y,
                         int x, int *scroll_offset, int *lines_to_print,
                         int editor_height, int filename_len,
                         bool *file_modified)
{
    if (x + 1 < tbuf->current_line->length)
    {
        memmove(&tbuf->current_line->buf_[x], &tbuf->current_line->buf_[x + 1],
                tbuf->current_line->length - x);
        tbuf->current_line->length--;
        mvwprintw(*edit_window, y, tbuf->current_line->length - 1, " ");
        // print_line(tbuf->current_line->buf_,
        //            tbuf->curr_line_nr - *scroll_offset, edit_window);
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        // log_values(edit_window, *scroll_offset, tbuf, 0, y, x);
        print_cursor_position(editor_window, tbuf, editor_height);
        if (file_modified)
        {
            print_modified_marker(*editor_window, filename_len, file_modified);
        }
        wnoutrefresh(*editor_window);
        wmove(*edit_window, y, x);
        wnoutrefresh(*edit_window);
    }
    else
    {
        delete_line(tbuf, edit_window, editor_window, line_num_win, y, x,
                    scroll_offset, lines_to_print, editor_height, filename_len,
                    file_modified);
    }
}

void bs_delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                    WINDOW **editor_window, WINDOW **line_num_win, int y,
                    int *scroll_offset, int *lines_to_print, int editor_height,
                    int filename_len, bool *file_modified)
{
    curs_set(0);
    memmove(
        &tbuf->current_line->prev->buf_[tbuf->current_line->prev->length - 1],
        tbuf->current_line->buf_, tbuf->current_line->length);
    tbuf->current_line->prev->next = tbuf->current_line->next;
    int prev_length = tbuf->current_line->prev->length;
    if (y + 1 < tbuf->num_of_lines - *scroll_offset)
    {
        tbuf->current_line->next->prev = tbuf->current_line->prev;
    }
    free(tbuf->current_line->buf_);
    tbuf->current_line->prev->length += tbuf->current_line->length - 1;
    tbuf->current_line = tbuf->current_line->prev;
    tbuf->num_of_lines--;
    tbuf->curr_line_nr--;
    tbuf->current_col = tbuf->current_line->length;
    if (tbuf->num_of_lines - *scroll_offset < editor_height - 7 &&
        *scroll_offset + *lines_to_print > tbuf->num_of_lines)
        *lines_to_print -= 1;
    // else
    //     *scroll_offset -= 1;
    print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                 *lines_to_print);
    // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y, 0);
    print_cursor_position(editor_window, tbuf, editor_height);
    if (file_modified)
    {
        print_modified_marker(*editor_window, filename_len, file_modified);
    }
    wnoutrefresh(*editor_window);
    wmove(*edit_window, y - 1, prev_length < 2 ? 0 : prev_length - 1);
    curs_set(2);
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}

void bs_delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                            WINDOW **edit_window, WINDOW **editor_window, int y,
                            int x, int *scroll_offset, int *lines_to_print,
                            int editor_height, int filename_len,
                            bool *file_modified)
{
    if (x > 0)
    {
        memmove(&tbuf->current_line->buf_[x - 1], &tbuf->current_line->buf_[x],
                tbuf->current_line->length - x);
        memset(&tbuf->current_line->buf_[tbuf->current_line->length - 1], '\0',
               1);
        tbuf->current_line->length--;
        tbuf->current_col--;
        mvwprintw(*edit_window, y, tbuf->current_line->length - 1, " ");
        // print_line(tbuf->current_line->buf_,
        //            tbuf->curr_line_nr - *scroll_offset, edit_window);
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        print_cursor_position(editor_window, tbuf, editor_height);
        if (file_modified)
        {
            print_modified_marker(*editor_window, filename_len, file_modified);
        }
        wnoutrefresh(*editor_window);
        wmove(*edit_window, y, x - 1);
        wnoutrefresh(*edit_window);
        doupdate();
    }
    else if (x == 0 && y > 0)
    {
        bs_delete_line(tbuf, edit_window, editor_window, line_num_win, y,
                       scroll_offset, lines_to_print, editor_height,
                       filename_len, file_modified);
    }
}

void insert_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                 WINDOW **editor_window, WINDOW **line_num_win, int y, int x,
                 int *scroll_offset, int *lines_to_print, int editor_height,
                 int filename_len, bool *file_modified)
{
    LINE *new_line = initialize_line();
    if (x + 1 < tbuf->current_line->length)
    {
        curs_set(0);
        memmove(new_line->buf_, &tbuf->current_line->buf_[x],
                tbuf->current_line->length - x);
        strncpy(&tbuf->current_line->buf_[x], "\n", 2 * sizeof(char));
        strncpy(&tbuf->current_line->buf_[x + 1], "\0",
                (tbuf->current_line->length - x) * sizeof(char));
        new_line->length = tbuf->current_line->length - x;

        new_line->prev = tbuf->current_line;
        if (y + 1 < tbuf->num_of_lines - *scroll_offset)
        {
            new_line->next = tbuf->current_line->next;
            tbuf->current_line->next->prev = new_line;
        }
        tbuf->current_line->next = new_line;
        tbuf->current_line->length = x + 1;
        tbuf->current_line = tbuf->current_line->next;
        tbuf->num_of_lines++;
        tbuf->curr_line_nr++;
        tbuf->current_col = 0;
        if (*lines_to_print < editor_height - 7)
        {
            *lines_to_print += 1;
        }
        else if (tbuf->curr_line_nr - *scroll_offset >= editor_height - 7)
        {
            *scroll_offset += 1;
        }
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        if (file_modified)
        {
            print_modified_marker(*editor_window, filename_len, file_modified);
        }
        wnoutrefresh(*editor_window);
        // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y, x);
        if (y < editor_height - 8)
        {
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y + 1, 0);
        }
        else
        {
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y, 0);
        }
        curs_set(2);
        wnoutrefresh(*edit_window);
        doupdate();
    }
    else
    {
        curs_set(0);
        strncpy(new_line->buf_, "\n\0", 2 * sizeof(char));
        new_line->length = 1;

        new_line->prev = tbuf->current_line;
        if (y + 1 < tbuf->num_of_lines - *scroll_offset)
        {
            new_line->next = tbuf->current_line->next;
            tbuf->current_line->next->prev = new_line;
        }
        tbuf->current_line->next = new_line;
        tbuf->current_line = tbuf->current_line->next;
        tbuf->num_of_lines++;
        tbuf->curr_line_nr++;
        tbuf->current_col = 0;
        if (*lines_to_print < editor_height - 7)
        {
            *lines_to_print += 1;
        }
        else if (tbuf->curr_line_nr - *scroll_offset >= editor_height - 7)
        {
            *scroll_offset += 1;
        }
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        if (file_modified)
        {
            print_modified_marker(*editor_window, filename_len, file_modified);
        }
        wnoutrefresh(*editor_window);
        // log_values(edit_window, *scroll_offset, tbuf, *lines_to_print, y, x);
        if (y < editor_height - 8 && *lines_to_print < editor_height - 6)
        {
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y + 1, 0);
        }
        else
        {
            print_cursor_position(editor_window, tbuf, editor_height);
            wmove(*edit_window, y, 0);
        }
        curs_set(2);
    }
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}

void log_values(WINDOW **edit_window, int scroll_offset, TEXT_BUFFER *tbuf,
                int lines_to_print, int y, int x, int editor_height)
{
    mvwprintw(*edit_window, editor_height - 13, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 12, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 11, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 10, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 9, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 8, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 13, EDIT_MAX - 15, "y:  %i", y);
    mvwprintw(*edit_window, editor_height - 12, EDIT_MAX - 15, "x:  %i", x);
    mvwprintw(*edit_window, editor_height - 11, EDIT_MAX - 15, "so:  %i",
              scroll_offset);
    mvwprintw(*edit_window, editor_height - 10, EDIT_MAX - 15, "nol: %i",
              tbuf->num_of_lines);
    mvwprintw(*edit_window, editor_height - 9, EDIT_MAX - 15, "ltp: %i",
              lines_to_print);
    mvwprintw(*edit_window, editor_height - 8, EDIT_MAX - 15, "cln: %i",
              tbuf->curr_line_nr);
}

void print_cursor_position(WINDOW **editor_window, TEXT_BUFFER *tbuf, int editor_height)
{
    double percent = 1.0 * tbuf->curr_line_nr / tbuf->num_of_lines;
    int pc = round(percent * 100);
    int offset = 8;
    if (pc > 9)
        offset++;
    if (tbuf->curr_line_nr > 8)
        offset++;
    if (tbuf->current_col > 8)
        offset++;
    if (pc > 99)
        offset++;
    if (tbuf->curr_line_nr > 98)
        offset++;
    if (tbuf->current_col > 98)
        offset++;

    mvwprintw(*editor_window, editor_height - 2, EDITOR_WIDTH - 18, "                  ");
    mvwprintw(*editor_window, editor_height - 2, EDITOR_WIDTH - offset,
              "%i%% %i:%i", pc, tbuf->curr_line_nr + 1, tbuf->current_col + 1);
    wnoutrefresh(*editor_window);
}
