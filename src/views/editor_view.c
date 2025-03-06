#include "../models/models.h"
#include <ncurses.h>

void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win)
{
    int i;
    for (i = 0; i < tbuf->num_of_lines; i++)
    {
        wattron(*line_num_win, COLOR_PAIR(4));
        if (i == tbuf->curr_line_nr)
        {
            wattroff(*line_num_win, COLOR_PAIR(4));
            wattron(*line_num_win, COLOR_PAIR(5));
        }

        if (i < 9)
        {
            mvwprintw(*line_num_win, i, 1, "%i", i + 1);
        }
        else
        {
            mvwprintw(*line_num_win, i, 0, "%i", i + 1);
        }

        if (i == tbuf->curr_line_nr)
        {
            wattroff(*line_num_win, COLOR_PAIR(5));
            wattron(*line_num_win, COLOR_PAIR(4));
        }
    }
    wattroff(*line_num_win, COLOR_PAIR(4));
}

void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win)
{
    wclear(*line_num_win);
    wclear(*edit_window);
    int i;

    LINE *next_line = tbuf->first_line;

    for (i = 0; i < tbuf->num_of_lines; i++)
    {
        mvwprintw(*edit_window, i, 0, "%s", next_line->buf_);
        next_line = next_line->next;
    }
    wattroff(*line_num_win, COLOR_PAIR(4));
    update_line_numbers(tbuf, line_num_win);
}
