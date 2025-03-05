#include "../models/models.h"
#include <ncurses.h>

void update_edit_window(CHAR_BUFFER *cbuf, LINE_BUFFER *lbuf,
                        int *new_file_size, WINDOW *line_num_win,
                        WINDOW *edit_window)
{
    wclear(edit_window);
    int i, c, y, x;
    i = c = y = x = 0;
    int file_size = 144;

    int first_buf_len = cbuf->ccur_ - cbuf->buf_;
    int start_second_buf = first_buf_len + cbuf->gap_size_;
    int second_buf_len = &cbuf->buf_[file_size] - cbuf->cend_;

    mvwprintw(edit_window, LINES - 14, 2, "Before");
    mvwprintw(edit_window, LINES - 13, 23, "%s", "  ");
    mvwprintw(edit_window, LINES - 13, 2, "cursor at: %li",
              cbuf->ccur_ - cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 8, "%s", "  ");
    mvwprintw(edit_window, LINES - 12, 2, "ccur: %p", cbuf->ccur_);
    mvwprintw(edit_window, LINES - 11, 8, "%s", "  ");
    mvwprintw(edit_window, LINES - 11, 2, "cend: %p", cbuf->cend_);

    mvwprintw(edit_window, LINES - 10, 23, "%s", "  ");
    mvwprintw(edit_window, LINES - 10, 2, "First buffer length: %i",
              first_buf_len);
    mvwprintw(edit_window, LINES - 9, 2, "Second buffer length: %i",
              second_buf_len);
    mvwprintw(edit_window, LINES - 8, 52, "Num of lines: %i", lbuf->size_);

    mvwprintw(edit_window, LINES - 16, 50, "%s", "  ");
    mvwprintw(edit_window, LINES - 16, 50, "%i", first_buf_len);
    mvwprintw(edit_window, LINES - 15, 50, "%s", "  ");
    mvwprintw(edit_window, LINES - 15, 50, "gap_size: %i", cbuf->gap_size_);
    mvwprintw(edit_window, LINES - 14, 50, "%s", "  ");
    mvwprintw(edit_window, LINES - 14, 50, "%i", start_second_buf);
    mvwprintw(edit_window, LINES - 13, 50, "%s", "  ");
    mvwprintw(edit_window, LINES - 13, 50, "%i", second_buf_len);
    mvwprintw(edit_window, LINES - 13, 50, "%s", "  ");
    // mvwprintw(edit_window, LINES - 13, 50, "%i", lbuf->size_);
    mvwprintw(edit_window, 20, 20, "%c", cbuf->buf_[65]);
    mvwprintw(edit_window, 21, 20, "%i", (file_size + cbuf->gap_size_));
    int lcur = lbuf->ccur_ - lbuf->new_lines_;
    mvwprintw(edit_window, 22, 20, "line cursor: %i", lcur);

    for (i = 0; i < *new_file_size; i++)
    {
        if (y < lbuf->size_ && y < 9)
        {
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(line_num_win, y + 1, 1, "%i", y + 1);
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(edit_window, LINES - 29 + y, 2, "%i",
                      lbuf->line_size_[y]);
        }
        else if (y < lbuf->size_ && y >= 9)
        {
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(line_num_win, y + 1, 0, "%i", y + 1);
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(edit_window, LINES - 29 + y, 2, "%i",
                      lbuf->line_size_[y]);
        }
        // mvwprintw(edit_window, i, l + 42, "%i", k);

        if (c == first_buf_len)
        {
            c = start_second_buf;
        }
        if (cbuf->buf_[c] == '\n')
        {
            x = 0;
            mvwprintw(edit_window, LINES - 32 + y, 75, "line: %i",
                      lbuf->new_lines_[y + (y < lcur ? 0 : lbuf->gap_size_)]);
            y++;
            mvwprintw(edit_window, y, x, "%c", cbuf->buf_[c]);
        }
        else
        {
            mvwprintw(edit_window, y, x, "%c", cbuf->buf_[c]);
            x++;
        }
        c++;
    }
    wrefresh(line_num_win);
}

void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win)
{
    wclear(*edit_window);
    int i;

    LINE *next_line = tbuf->first_line;

    wattron(*line_num_win, COLOR_PAIR(4));
    for (i = 0; i < tbuf->num_of_lines; i++)
    {
        if (i < 9)
        {
            mvwprintw(*line_num_win, i, 1, "%i", i + 1);
        }
        else
        {
            mvwprintw(*line_num_win, i, 0, "%i", i + 1);
        }
        mvwprintw(*edit_window, i, 0, "%s", next_line->buf_);
        next_line = next_line->next;
    }
    wattroff(*line_num_win, COLOR_PAIR(4));
}
