#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

void move_down(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
               int y, int x)
{
    if (tbuf->curr_line_nr + 1 >= tbuf->num_of_lines)
        return;

    if (x + 1 < tbuf->current_line->next->length)
    {
        mvwprintw(*edit_window, LINES - 9, 70, "   ");
        mvwprintw(*edit_window, LINES - 9, 70, "%i",
                  tbuf->current_line->next->length);
        tbuf->current_line = tbuf->current_line->next;
        tbuf->curr_line_nr++;
        wmove(*edit_window, y + 1, x);
    }
    else
    {
        mvwprintw(*edit_window, LINES - 9, 70, "   ");
        mvwprintw(*edit_window, LINES - 9, 70, "%i",
                  tbuf->current_line->next->length);
        tbuf->current_line = tbuf->current_line->next;
        tbuf->curr_line_nr++;
        tbuf->current_col = tbuf->current_line->length - 1;
        wmove(*edit_window, y + 1, tbuf->current_line->length - 1);
    }
    update_line_numbers(tbuf, line_num_win);
    wrefresh(*line_num_win);
    wrefresh(*edit_window);
}

void move_up(TEXT_BUFFER *tbuf, WINDOW **line_num_win, WINDOW **edit_window,
             int y, int x)
{
    if (tbuf->curr_line_nr == 0)
        return;

    if (x + 1 < tbuf->current_line->prev->length)
    {
        mvwprintw(*edit_window, LINES - 9, 70, "   ");
        mvwprintw(*edit_window, LINES - 9, 70, "%i",
                  tbuf->current_line->prev->length);
        tbuf->current_line = tbuf->current_line->prev;
        tbuf->curr_line_nr--;
        wmove(*edit_window, y - 1, x);
    }
    else
    {
        mvwprintw(*edit_window, LINES - 9, 70, "   ");
        mvwprintw(*edit_window, LINES - 9, 70, "%i",
                  tbuf->current_line->prev->length);
        tbuf->current_line = tbuf->current_line->prev;
        tbuf->curr_line_nr--;
        tbuf->current_col = tbuf->current_line->length - 1;
        wmove(*edit_window, y - 1, tbuf->current_line->length - 1);
    }
    update_line_numbers(tbuf, line_num_win);
    wrefresh(*line_num_win);
    wrefresh(*edit_window);
}

void move_right(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    if (x + 1 < tbuf->current_line->length)
    {
        tbuf->current_col++;
        wmove(*edit_window, y, x + 1);
        wrefresh(*edit_window);
    }
}

void move_left(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    if (x > 0)
    {
        tbuf->current_col--;
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
        wmove(*edit_window, y, 0);
        wrefresh(*edit_window);
    }
}

void move_to_end_of_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    if (x < tbuf->current_line->length - 1)
    {
        tbuf->current_col = tbuf->current_line->length - 1;
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
    mvwprintw(*edit_window, y, x, "%s", &tbuf->current_line->buf_[x]);
    wmove(*edit_window, y, x + 1);
    wrefresh(*edit_window);
}

void delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **line_num_win,
                 int y, int x)
{
    if (y + 1 < tbuf->num_of_lines)
    {
        memmove(
            &tbuf->current_line->buf_[tbuf->current_line->length - 1],
            tbuf->current_line->next->buf_, tbuf->current_line->next->length);
        // memmove(
        //     &tbuf->current_line->prev->buf_[tbuf->current_line->prev->length - 1],
        //     tbuf->current_line->buf_, tbuf->current_line->length);
        // tbuf->current_line->prev->next = tbuf->current_line->next;
        // int prev_length = tbuf->current_line->prev->length;
        free(tbuf->current_line->next->buf_);
        tbuf->current_line->length += (tbuf->current_line->next->length - 1);
        tbuf->current_line->next->next->prev = tbuf->current_line;
        tbuf->current_line->next = tbuf->current_line->next->next;
        // tbuf->current_line = tbuf->current_line->prev;
        tbuf->num_of_lines--;
        // tbuf->curr_line_nr--;
        // tbuf->current_col = tbuf->current_line->length;
        print_buffer(tbuf, edit_window, line_num_win);
        wmove(*edit_window, y, x);
        wrefresh(*line_num_win);
        wrefresh(*edit_window);
    }
}

void delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         WINDOW **edit_window, int y, int x)
{
    if (x + 2 < tbuf->current_line->length)
    {
        memmove(&tbuf->current_line->buf_[x], &tbuf->current_line->buf_[x + 1],
                tbuf->current_line->length - x);
        // memset(&tbuf->current_line->buf_[tbuf->current_line->length - 1],
        // '\n', 1);
        // strncpy(&tbuf->current_line->buf_[tbuf->current_line->length - 1],
        //         "\n", sizeof(char));
        tbuf->current_line->length--;
        mvwprintw(*edit_window, y, tbuf->current_line->length - 1, " ");
        mvwprintw(*edit_window, y, x, "%s", &tbuf->current_line->buf_[x]);
        mvwprintw(*edit_window, LINES - 20, 70, "char deleted");
        wmove(*edit_window, y, x);
        wrefresh(*edit_window);
    }
    else
    {
        delete_line(tbuf, edit_window, line_num_win, y, x);
    }
}

void bs_delete_line(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                    WINDOW **line_num_win, int y)
{
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
    print_buffer(tbuf, edit_window, line_num_win);
    wmove(*edit_window, y - 1, prev_length < 2 ? 0 : prev_length - 1);
    wrefresh(*line_num_win);
    wrefresh(*edit_window);
}

void bs_delete_char_or_line(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                            WINDOW **edit_window, int y, int x)
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
        mvwprintw(*edit_window, y, x - 1, "%s",
                  &tbuf->current_line->buf_[x - 1]);
        wmove(*edit_window, y, x - 1);
        wrefresh(*edit_window);
    }
    else if (x == 0 && y > 0)
    {
        bs_delete_line(tbuf, edit_window, line_num_win, y);
    }
}

void insert_line(TEXT_BUFFER *tbuf, WINDOW **edit_window, WINDOW **line_num_win,
                 int y, int x)
{
    LINE *new_line = initialize_line();
    if (x + 1 < tbuf->current_line->length)
    {
        memmove(new_line->buf_, &tbuf->current_line->buf_[x],
                tbuf->current_line->length - x);
        strncpy(&tbuf->current_line->buf_[x], "\n\0", 2 * sizeof(char));
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
        print_buffer(tbuf, edit_window, line_num_win);
        wmove(*edit_window, y + 1, 0);
    }
    else
    {
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
        print_buffer(tbuf, edit_window, line_num_win);
        wmove(*edit_window, y + 1, 0);
    }
}
