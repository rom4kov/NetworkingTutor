#include "../models/models.h"
#include <ncurses.h>
#include <string.h>

void move_down(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
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
    wrefresh(*edit_window);
}

void move_up(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
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

void insert_char(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x, char ch)
{
    memmove(&tbuf->current_line->buf_[x + 1], &tbuf->current_line->buf_[x],
            tbuf->current_line->length - x - 1);
    tbuf->current_line->length++;
    tbuf->current_line->buf_[x] = ch;
    tbuf->current_col++;
    mvwprintw(*edit_window, y, x, "%s", &tbuf->current_line->buf_[x]);
    wmove(*edit_window, y, x + 1);
    wrefresh(*edit_window);
}

void delete_char(TEXT_BUFFER *tbuf, WINDOW **edit_window, int y, int x)
{
    memmove(&tbuf->current_line->buf_[x - 1], &tbuf->current_line->buf_[x],
            tbuf->current_line->length - x);
    memset(&tbuf->current_line->buf_[tbuf->current_line->length - 1], '\0', 1);
    tbuf->current_line->length--;
    tbuf->current_col--;
    mvwprintw(*edit_window, y, x - 1, "%s", &tbuf->current_line->buf_[x - 1]);
    mvwprintw(*edit_window, y, tbuf->current_line->length - 1, " ");
    wmove(*edit_window, y, x - 1);
    wrefresh(*edit_window);
}
