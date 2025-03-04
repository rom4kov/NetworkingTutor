#include "controllers.h"
#include "../models/models.h"
#include <string.h>

LINE *insert_new_line(LINE *head)
{
    LINE *new_line = initialize_line();

    head->next = new_line;

    return new_line;
}

int get_line_length(LINE_BUFFER *lbuf, int line_index)
{
    int actual_index;

    if (line_index < (lbuf->ccur_ - lbuf->new_lines_))
    {
        actual_index = line_index;
    }
    else
    {
        actual_index = line_index + lbuf->gap_size_;
    }

    if (line_index == 0)
    {
        return lbuf->new_lines_[actual_index] + 1;
    }

    int prev_actual_index;
    if ((line_index - 1) < (lbuf->ccur_ - lbuf->new_lines_))
    {
        prev_actual_index = line_index - 1;
    }
    else
    {
        prev_actual_index = (line_index - 1) + lbuf->gap_size_;
    }

    return lbuf->new_lines_[actual_index] - lbuf->new_lines_[prev_actual_index];
}

int calculate_cursor_offset(LINE_BUFFER *lbuf, CHAR_BUFFER *cbuf, int curr_line,
                            int x, int pll, int cll, int nll, int direction)
{
    int cursor_offset;
    int line_feed_pos;

    if (curr_line < (lbuf->ccur_ - lbuf->new_lines_))
    {
        line_feed_pos = lbuf->new_lines_[curr_line + direction];
    }
    else
    {
        line_feed_pos =
            lbuf->new_lines_[curr_line + direction + lbuf->gap_size_];
    }

    if (direction == 1)
    {
        if (nll - 1 > x)
            cursor_offset = cll;
        else
        {
            cursor_offset =
                line_feed_pos - (cbuf->ccur_ - cbuf->buf_) - (nll < 2 ? 0 : 1);
        }
    }
    else
    {
        if (pll - 1 > x)
            cursor_offset = pll;
        else
        {
            cursor_offset =
                (cbuf->ccur_ - cbuf->buf_) - line_feed_pos + (pll < 2 ? 0 : 1);
        }
    }

    return cursor_offset;
}

void move_gap_to_line(LINE_BUFFER *lbuf, int line_index)
{
    int current_pos = lbuf->ccur_ - lbuf->new_lines_;

    if (line_index == current_pos)
    {
        return;
    }

    if (line_index < current_pos)
    {
        int count = current_pos - line_index;
        memmove(lbuf->cend_ - count, lbuf->new_lines_ + line_index,
                count * sizeof(int));
    }
    else
    {
        int count = line_index - current_pos;
        memmove(lbuf->ccur_, lbuf->cend_, count * sizeof(int));
    }

    lbuf->ccur_ = lbuf->new_lines_ + line_index;
    lbuf->cend_ = lbuf->ccur_ + lbuf->gap_size_;
}

// void insert_new_line(LINE_BUFFER *lbuf, CHAR_BUFFER *cbuf)
// {
//     if (lbuf->gap_size_ <= 0)
//     {
//     }
//
//     *(lbuf->ccur_) = cbuf->ccur_ - cbuf->buf_;
//
//     lbuf->ccur_++;
//     lbuf->gap_size_--;
//     lbuf->size_++;
// }

void update_lines(LINE_BUFFER *lbuf, int mode)
{
    int i;
    int gap_end = lbuf->cend_ - lbuf->new_lines_;
    int last_line = lbuf->size_ + lbuf->gap_size_;

    for (i = gap_end; i < last_line; i++)
    {
        lbuf->new_lines_[i] += mode;
    }
}
