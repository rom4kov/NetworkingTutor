#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDIT_MAX WU * 5 + (WU / 2) + 4

void move_down(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
               int y, int x, int *scroll_offset, int lines_to_print)
{
    if (tbuf->curr_line_nr + 1 >= tbuf->num_of_lines)
        return;

    if (x + 1 < tbuf->current_line->next->length)
    {
        tbuf->current_line = tbuf->current_line->next;
        tbuf->curr_line_nr++;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr > *scroll_offset + LINES - 8)
        {
            curs_set(0);
            *scroll_offset += 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         lines_to_print);
            wmove(*edit_window, y, x);
            curs_set(1);
        }
        else
            wmove(*edit_window, y + 1, x);
    }
    else
    {
        tbuf->current_line = tbuf->current_line->next;
        tbuf->curr_line_nr++;
        tbuf->current_col = tbuf->current_line->length - 1;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr > *scroll_offset + LINES - 8)
        {
            curs_set(0);
            *scroll_offset += 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         lines_to_print);
            wmove(*edit_window, y, tbuf->current_line->length - 1);
            curs_set(1);
        }
        else
            wmove(*edit_window, y + 1, tbuf->current_line->length - 1);
    }
    update_line_numbers(tbuf, line_num_win, scroll_offset, lines_to_print);
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}

void move_up(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
             int y, int x, int *scroll_offset, int lines_to_print)
{
    if (tbuf->curr_line_nr == 0)
        return;

    if (x + 1 < tbuf->current_line->prev->length)
    {
        tbuf->current_line = tbuf->current_line->prev;
        tbuf->curr_line_nr--;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr < *scroll_offset)
        {
            curs_set(0);
            *scroll_offset -= 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         lines_to_print);
            wmove(*edit_window, y, x);
            curs_set(1);
        }
        else
            wmove(*edit_window, y - 1, x);
    }
    else
    {
        tbuf->current_line = tbuf->current_line->prev;
        tbuf->curr_line_nr--;
        tbuf->current_col = tbuf->current_line->length - 1;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        if (tbuf->curr_line_nr < *scroll_offset)
        {
            curs_set(0);
            *scroll_offset -= 1;
            print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                         lines_to_print);
            wmove(*edit_window, y, tbuf->current_line->length - 1);
            curs_set(1);
        }
        else
            wmove(*edit_window, y - 1, tbuf->current_line->length - 1);
    }
    update_line_numbers(tbuf, line_num_win, scroll_offset, lines_to_print);
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}

void move_right(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    if (x + 1 < tbuf->current_line->length)
    {
        tbuf->current_col++;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        wmove(*edit_window, y, x + 1);
        wrefresh(*edit_window);
    }
}

void move_left(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    if (x > 0)
    {
        tbuf->current_col--;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        wmove(*edit_window, y, x - 1);
        wrefresh(*edit_window);
    }
}

void move_to_start_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y,
                           int x)
{
    if (x > 0)
    {
        tbuf->current_col = 0;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        wmove(*edit_window, y, 0);
        wrefresh(*edit_window);
    }
}

void move_to_end_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    if (x < tbuf->current_line->length - 1)
    {
        tbuf->current_col = tbuf->current_line->length - 1;
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "       ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "%i : %i",
                  tbuf->curr_line_nr, tbuf->current_col);
        wmove(*edit_window, y, tbuf->current_line->length - 1);
        wrefresh(*edit_window);
    }
}

void insert_char(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x, char ch)
{
    memmove(&tbuf->current_line->buf_[x + 1], &tbuf->current_line->buf_[x],
            tbuf->current_line->length - x);
    tbuf->current_line->length++;
    tbuf->current_line->buf_[x] = ch;
    tbuf->current_col++;
    print_line(tbuf->current_line->buf_, tbuf->curr_line_nr, edit_window);
    wmove(*edit_window, y, x + 1);
    wrefresh(*edit_window);
}

void insert_tab(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    int move_len = 4 - ((x + 4) % 4);
    memmove(&tbuf->current_line->buf_[x + move_len],
            &tbuf->current_line->buf_[x], tbuf->current_line->length - x);
    tbuf->current_line->length += move_len;
    memset(&tbuf->current_line->buf_[x], 32, move_len * sizeof(char));
    tbuf->current_col += move_len;
    print_line(tbuf->current_line->buf_, tbuf->curr_line_nr, edit_window);
    wmove(*edit_window, y, x + move_len);
    wrefresh(*edit_window);
}

void delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **line_num_win,
                 int y, int x, int *scroll_offset, int *lines_to_print)
{
    if (y + 1 < tbuf->num_of_lines)
    {
        memmove(&tbuf->current_line->buf_[tbuf->current_line->length - 1],
                tbuf->current_line->next->buf_,
                tbuf->current_line->next->length);
        free(tbuf->current_line->next->buf_);
        tbuf->current_line->length += (tbuf->current_line->next->length - 1);
        tbuf->current_line->next->next->prev = tbuf->current_line;
        tbuf->current_line->next = tbuf->current_line->next->next;
        tbuf->num_of_lines--;
        curs_set(0);
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset, *lines_to_print);
        wmove(*edit_window, y, x);
        curs_set(1);
        wnoutrefresh(*line_num_win);
        wnoutrefresh(*edit_window);
        doupdate();
    }
}

void delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         WINDOW **edit_window, int y, int x, int *scroll_offset, int *lines_to_print)
{
    if (x + 2 < tbuf->current_line->length)
    {
        memmove(&tbuf->current_line->buf_[x], &tbuf->current_line->buf_[x + 1],
                tbuf->current_line->length - x);
        tbuf->current_line->length--;
        mvwprintw(*edit_window, y, tbuf->current_line->length - 1, " ");
        print_line(tbuf->current_line->buf_, tbuf->curr_line_nr, edit_window);
        wmove(*edit_window, y, x);
        wrefresh(*edit_window);
    }
    else
    {
        delete_line(tbuf, edit_window, line_num_win, y, x, scroll_offset, lines_to_print);
    }
}

void bs_delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                    WINDOW **line_num_win, int y, int *scroll_offset, int *lines_to_print)
{
    curs_set(0);
    memmove(
        &tbuf->current_line->prev->buf_[tbuf->current_line->prev->length - 1],
        tbuf->current_line->buf_, tbuf->current_line->length);
    tbuf->current_line->prev->next = tbuf->current_line->next;
    int prev_length = tbuf->current_line->prev->length;
    if (y + 1 < tbuf->num_of_lines)
    {
        tbuf->current_line->next->prev = tbuf->current_line->prev;
    }
    free(tbuf->current_line->buf_);
    tbuf->current_line->prev->length += tbuf->current_line->length - 1;
    tbuf->current_line = tbuf->current_line->prev;
    tbuf->num_of_lines--;
    tbuf->curr_line_nr--;
    tbuf->current_col = tbuf->current_line->length;
    print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                 *lines_to_print);
    wmove(*edit_window, y - 1, prev_length < 2 ? 0 : prev_length - 1);
    curs_set(1);
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}

void bs_delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                            WINDOW **edit_window, int y, int x,
                            int *scroll_offset, int *lines_to_print)
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
        print_line(tbuf->current_line->buf_, tbuf->curr_line_nr, edit_window);
        wmove(*edit_window, y, x - 1);
        wrefresh(*edit_window);
    }
    else if (x == 0 && y > 0)
    {
        bs_delete_line(tbuf, edit_window, line_num_win, y, scroll_offset, lines_to_print);
    }
}

void insert_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **line_num_win,
                 int y, int x, int *scroll_offset, int *lines_to_print)
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
        if (y + 1 < tbuf->num_of_lines)
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
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        wmove(*edit_window, y + 1, 0);
        curs_set(1);
    }
    else
    {
        curs_set(0);
        strncpy(new_line->buf_, "\n\0", 2 * sizeof(char));
        new_line->length = 1;
        new_line->prev = tbuf->current_line;
        if (y + 1 < tbuf->num_of_lines)
        {
            new_line->next = tbuf->current_line->next;
            tbuf->current_line->next->prev = new_line;
        }
        tbuf->current_line->next = new_line;
        tbuf->current_line = tbuf->current_line->next;
        tbuf->num_of_lines++;
        tbuf->curr_line_nr++;
        tbuf->current_col = 0;
        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        wmove(*edit_window, y + 1, 0);
        curs_set(1);
    }
    wnoutrefresh(*line_num_win);
    wnoutrefresh(*edit_window);
    doupdate();
}
