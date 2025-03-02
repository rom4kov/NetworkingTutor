#define _XOPEN_SOURCE 500

#include "../core/core.h"
#include "../models/models.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void write_buffer_to_file(CHAR_BUFFER *cbuf, FILE *file, int file_size,
                          int *new_file_size, WINDOW *edit_window, char *ccur,
                          char *cend, LINE_BUFFER *lbuf)
{
    for (int i = 0; i < lbuf->size_; i++)
    {
        mvwprintw(edit_window, LINES - 29 + i, 6, "%i", lbuf->line_size_[i]);
    }

    int first_buf_len = cbuf->ccur_ - cbuf->buf_;
    int second_buf_len =
        &cbuf->buf_[*new_file_size + cbuf->gap_size_] - cbuf->cend_;
    mvwprintw(edit_window, LINES - 16, 52, "After:");
    mvwprintw(edit_window, LINES - 15, 52, "Mod size: %i", cbuf->mod_size_);
    // mvwprintw(edit_window, LINES - 14, 52, "File pointer: %li", ftell(file));
    mvwprintw(edit_window, LINES - 13, 52, "First byte of buffer: %p",
              cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 52, "ccur: %p", ccur);
    mvwprintw(edit_window, LINES - 11, 52, "cend: %p", cend);

    mvwprintw(edit_window, LINES - 10, 52, "First buffer length: %i",
              first_buf_len);
    mvwprintw(edit_window, LINES - 9, 52, "Second buffer length: %i",
              second_buf_len);

    fwrite(cbuf->buf_, sizeof(char), first_buf_len, file);
    fwrite(cbuf->cend_, sizeof(char), second_buf_len, file);

    rewind(file);

    int first_lbuf_len = lbuf->ccur_ - lbuf->new_lines_;
    int second_lbuf_len =
        &lbuf->new_lines_[lbuf->size_ + lbuf->gap_size_] - lbuf->cend_;
    FILE *lines_file = fopen("../lines.txt", "r+");
    fwrite(lbuf->new_lines_, sizeof(int), first_lbuf_len, lines_file);
    fwrite(lbuf->cend_, sizeof(int), second_lbuf_len, lines_file);

    // int chars_diff = *new_file_size - file_size;
    mvwprintw(edit_window, LINES - 20, 52, "file_size: %i", file_size);
    mvwprintw(edit_window, LINES - 21, 52, "new_file_size: %i", *new_file_size);
    // mvwprintw(edit_window, LINES - 22, 52, "chars_diff: %i", chars_diff);
    // fseek(file, chars_diff, SEEK_END);
    // off_t position = ftell(file);
    // mvwprintw(edit_window, LINES - 24, 52, "position: %li", position);
    // const char *path = "../hello.c";
    // int rc;
    // if ((rc = ftruncate(fileno(file), position)) == 0)
    // {
    //     mvwprintw(edit_window, LINES - 16, 70, "Truncated");
    // }
    // else
    // {
    //     mvwprintw(edit_window, LINES - 16, 70, "Error: %i", rc);
    // }

    wrefresh(edit_window);
}

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
    mvwprintw(edit_window, LINES - 13, 2, "First byte of buffer: %p",
              cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 8, "%s", "  ");
    mvwprintw(edit_window, LINES - 12, 2, "ccur: %p", cbuf->ccur_);
    mvwprintw(edit_window, LINES - 11, 8, "%s", "  ");
    mvwprintw(edit_window, LINES - 11, 2, "cend: %p", cbuf->cend_);

    mvwprintw(edit_window, LINES - 10, 23, "%s", "  ");
    mvwprintw(edit_window, LINES - 10, 2, "First buffer length: %i",
              first_buf_len);
    mvwprintw(edit_window, LINES - 9, 2, "Second buffer length: %i",
              second_buf_len);

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
    mvwprintw(edit_window, 22, 20, "%i", lcur);

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
            mvwprintw(edit_window, LINES - 32 + y, 75, "%i",
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

void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         int y, int x, CHAR_BUFFER *char_buf,
                         LINE_BUFFER *line_buf, bool *editor_mode, FILE *file,
                         int file_size, int *new_file_size)
{
    int curr_line = line_buf->ccur_ - line_buf->new_lines_;
    // int curr_line_len =
    //     line_buf->new_lines_[line_buf->cend_ - line_buf->new_lines_] -
    //     line_buf->new_lines_[line_buf->cend_ - line_buf->new_lines_ - 1] + 1;
    // int next_line_len =
    //     line_buf->new_lines_[line_buf->cend_ - line_buf->new_lines_ + 1] -
    //     line_buf->new_lines_[line_buf->cend_ - line_buf->new_lines_] + 1;
    // int prev_line_len =
    //     curr_line > 0
    //         ? line_buf->new_lines_[curr_line + line_buf->gap_size_ - 1] -
    //               line_buf->new_lines_[curr_line + line_buf->gap_size_ - 2] +
    //               1
    //         : 1;

    int curr_line_len = get_line_length(line_buf, curr_line);
    int next_line_len = get_line_length(line_buf, curr_line + 1);
    int prev_line_len = get_line_length(line_buf, curr_line - 1);

    // int prev_line_len =
    //     curr_line > 0
    //         ? line_buf->new_lines_[line_buf->cend_ - line_buf->new_lines_ -
    //         1] -
    //               line_buf
    //                   ->new_lines_[line_buf->cend_ - line_buf->new_lines_ -
    //                   2]
    //         : 0;

    int ccur_change_kdown;
    int ccur_change_kup;

    switch (ch)
    {
        case KEY_RIGHT:
            getyx(*edit_window, y, x);
            if (x + 1 < curr_line_len - 1)
            {
                char_buf->ccur_++;
                char_buf->cend_++;
                char_buf->mod_size_++;
                mvwprintw(*edit_window, LINES - 7, 70, "             ");
                mvwprintw(*edit_window, LINES - 7, 70, "Curr line: %i",
                          curr_line_len);
                mvwprintw(*edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(*edit_window, LINES - 7, 90, "y: %i", y);
                mvwprintw(*edit_window, LINES - 7, 96, "x: %i", x + 1);
                mvwprintw(*edit_window, LINES - 8, 90, "           ");
                mvwprintw(*edit_window, LINES - 8, 90, "curs: %li",
                          char_buf->ccur_ - char_buf->buf_);
                wmove(*edit_window, y, x);
                wmove(*edit_window, y, x + 1);
                wrefresh(*edit_window);
            }
            break;
        case KEY_LEFT:
            getyx(*edit_window, y, x);
            if (x > 0)
            {
                char_buf->ccur_--;
                char_buf->cend_--;
                char_buf->mod_size_--;
                mvwprintw(*edit_window, LINES - 7, 70, "             ");
                mvwprintw(*edit_window, LINES - 7, 70, "Curr line: %i",
                          curr_line_len);
                mvwprintw(*edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(*edit_window, LINES - 7, 90, "y: %i", y);
                mvwprintw(*edit_window, LINES - 7, 96, "x: %i", x - 1);
                mvwprintw(*edit_window, LINES - 8, 90, "           ");
                mvwprintw(*edit_window, LINES - 8, 90, "curs: %li",
                          char_buf->ccur_ - char_buf->buf_);
                wmove(*edit_window, y, x);
                wmove(*edit_window, y, x - 1);
                wrefresh(*edit_window);
            }
            break;
        case KEY_DOWN:
            if (line_buf->ccur_ - line_buf->new_lines_ + 1 < line_buf->size_)
            {
                getyx(*edit_window, y, x);
                if (next_line_len - 1 > x)
                    ccur_change_kdown = curr_line_len;
                else
                {
                    ccur_change_kdown =
                        // line_buf->new_lines_[curr_line + 1] - // 21
                        line_buf->new_lines_[curr_line + 1 +
                                             line_buf->gap_size_] - // 21
                        (char_buf->ccur_ - char_buf->buf_) -        // 0
                        (next_line_len < 2 ? 0 : 1);                // 1
                }

                char_buf->ccur_ += ccur_change_kdown;
                char_buf->cend_ += ccur_change_kdown;
                char_buf->mod_size_ += ccur_change_kdown;
                line_buf->ccur_++;
                line_buf->cend_++;
                line_buf->mod_size_++;
                mvwprintw(*edit_window, LINES - 9, 50, "             ");
                mvwprintw(*edit_window, LINES - 9, 50, "nll: %i",
                          next_line_len);
                mvwprintw(*edit_window, LINES - 7, 50, "             ");
                // mvwprintw(*edit_window, LINES - 7, 50, "Pr lb: %i",
                //           ++line_buf->new_lines_[curr_line - 1]);
                mvwprintw(*edit_window, LINES - 7, 70, "             ");
                mvwprintw(*edit_window, LINES - 7, 70, "Curr line: %i",
                          next_line_len);
                mvwprintw(*edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(*edit_window, LINES - 7, 90, "y: %i", y + 1);
                mvwprintw(*edit_window, LINES - 7, 96, "x: %i", x);
                mvwprintw(*edit_window, LINES - 7, 102, "          ");
                mvwprintw(*edit_window, LINES - 7, 102, "c mov: %i",
                          ccur_change_kdown);
                mvwprintw(*edit_window, LINES - 8, 90,
                          "                         ");
                mvwprintw(*edit_window, LINES - 9, 102, "pl idx: %i",
                          line_buf->new_lines_[curr_line - 1]);
                mvwprintw(
                    *edit_window, LINES - 8, 102, "nl idx: %i",
                    line_buf->new_lines_[curr_line + 1 + line_buf->gap_size_]);
                mvwprintw(*edit_window, LINES - 8, 90, "curs: %li",
                          char_buf->ccur_ - char_buf->buf_);
                wmove(*edit_window, y, x);
                if (next_line_len - 1 > x)
                    wmove(*edit_window, y + 1, x);
                else
                    wmove(*edit_window, y + 1,
                          next_line_len == 1 ? next_line_len - 1
                                             : next_line_len - 2);
                wrefresh(*edit_window);
            }
            break;
        case KEY_UP:
            if (curr_line > 0)
            {
                getyx(*edit_window, y, x);
                if (prev_line_len - 1 > x)
                    ccur_change_kup = prev_line_len;
                else
                {
                    ccur_change_kup =
                        (char_buf->ccur_ - char_buf->buf_) -
                        line_buf
                            ->new_lines_[curr_line - 1 + line_buf->gap_size_] +
                        (prev_line_len < 2 ? 0 : 1);
                }
                char_buf->ccur_ -= ccur_change_kup;
                char_buf->cend_ -= ccur_change_kup;
                char_buf->mod_size_ -= ccur_change_kup;
                line_buf->ccur_--;
                line_buf->cend_--;
                line_buf->mod_size_++;
                mvwprintw(*edit_window, LINES - 9, 50, "             ");
                mvwprintw(*edit_window, LINES - 9, 50, "nll: %i",
                          next_line_len);
                mvwprintw(*edit_window, LINES - 7, 70, "             ");
                mvwprintw(*edit_window, LINES - 7, 70, "Curr line: %i",
                          prev_line_len);
                mvwprintw(*edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(*edit_window, LINES - 7, 90, "y: %i", y - 1);
                mvwprintw(*edit_window, LINES - 7, 96, "x: %i", x);
                mvwprintw(*edit_window, LINES - 7, 102, "          ");
                mvwprintw(*edit_window, LINES - 7, 102, "c mov: %i",
                          ccur_change_kup);
                mvwprintw(*edit_window, LINES - 8, 90,
                          "                         ");
                mvwprintw(*edit_window, LINES - 9, 102, "pl nl idx: %i",
                          line_buf->new_lines_[curr_line - 1]);
                mvwprintw(
                    *edit_window, LINES - 8, 102, "nl idx: %i",
                    line_buf->new_lines_[curr_line + 1 + line_buf->gap_size_]);
                mvwprintw(*edit_window, LINES - 8, 90, "curs: %li",
                          char_buf->ccur_ - char_buf->buf_);
                wmove(*edit_window, y, x);
                if (prev_line_len > x && prev_line_len > 2)
                    wmove(*edit_window, y - 1, x);
                else if (prev_line_len <= x && prev_line_len > 2)
                    wmove(*edit_window, y - 1, prev_line_len - 2);
                else
                    wmove(*edit_window, y - 1, 0);
                wrefresh(*edit_window);
            }
            break;
        case KEY_BACKSPACE:
            if (char_buf->mod_size_ > 0)
                memmove(char_buf->ccur_ - char_buf->mod_size_,
                        char_buf->cend_ - char_buf->mod_size_,
                        char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memmove(char_buf->cend_, char_buf->ccur_,
                        abs(char_buf->mod_size_));
            // if (x == 0)
            // {
            // if (line_buf->mod_size_ > 0)
            //     memmove(&curr_line - line_buf->mod_size_,
            //            &line_buf->cend_ - line_buf->mod_size_,
            //            line_buf->mod_size_);
            // else if (line_buf->mod_size_ < 0)
            //     memmove(&line_buf->cend_, &curr_line,
            //            abs(line_buf->mod_size_));
            // line_buf->size_--;
            // line_buf->ccur_--;
            // line_buf->line_size_[curr_line] =
            //     line_buf->line_size_[curr_line] - 1;
            // update_lines(line_buf, -1);
            // }
            char_buf->mod_size_ = 0;
            *char_buf->ccur_ = ' ';
            char_buf->ccur_--;
            char_buf->gap_size_ += 1;
            *new_file_size -= 1;

            update_lines(line_buf, -1);
            getyx(*edit_window, y, x);
            // if (x == 0)
            // {
            //     wmove(*edit_window, y - 1, x);
            //     wrefresh(*edit_window);
            //     break;
            // }
            update_edit_window(char_buf, line_buf, new_file_size, *line_num_win,
                               *edit_window);
            wmove(*edit_window, y, x - 1);
            wrefresh(*edit_window);
            break;
        case 10:
            if (char_buf->mod_size_ > 0)
                memmove(char_buf->ccur_ - char_buf->mod_size_,
                        char_buf->cend_ - char_buf->mod_size_,
                        char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memmove(char_buf->cend_, char_buf->ccur_,
                        abs(char_buf->mod_size_));
            if (line_buf->mod_size_ > 0)
                memmove(line_buf->ccur_ - line_buf->mod_size_,
                        line_buf->cend_ - line_buf->mod_size_,
                        line_buf->mod_size_ * sizeof(int));
            else if (line_buf->mod_size_ < 0)
                memmove(line_buf->cend_, line_buf->ccur_,
                        abs(line_buf->mod_size_) * sizeof(int));
            *char_buf->ccur_ = '\n';
            char_buf->ccur_++;
            getyx(*edit_window, y, x);

            line_buf->new_lines_[curr_line] =
                curr_line < 1 ? x
                              : (line_buf->new_lines_[curr_line - 1] + x + 1);

            line_buf->ccur_++;
            line_buf->gap_size_--;
            line_buf->size_++;
            char_buf->gap_size_ -= 1;
            *new_file_size += 1;
            update_lines(line_buf, 1);
            update_edit_window(char_buf, line_buf, new_file_size, *line_num_win,
                               *edit_window);
            wmove(*edit_window, y + 1, 0);
            wrefresh(*edit_window);
            break;
        case KEY_F(1):
            curs_set(0);
            wrefresh(*edit_window);
            *editor_mode = false;
            break;
        case KEY_F(10):
            mvwprintw(*edit_window, LINES - 30, 52, "nfs before passing: %i",
                      *new_file_size);
            write_buffer_to_file(char_buf, file, file_size, new_file_size,
                                 *edit_window, char_buf->ccur_, char_buf->cend_,
                                 line_buf);
            break;
        default:
            if (char_buf->mod_size_ > 0)
                memmove(char_buf->ccur_ - char_buf->mod_size_,
                        char_buf->cend_ - char_buf->mod_size_,
                        char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memmove(char_buf->cend_, char_buf->ccur_,
                        abs(char_buf->mod_size_));
            *char_buf->ccur_ = ch;
            char_buf->ccur_++;
            char_buf->gap_size_ -= 1;
            // line_buf->line_size_[curr_line] =
            //     line_buf->line_size_[curr_line] + 1;
            // for (int i = curr_line; i < line_buf->size_; i++)
            // {
            //     line_buf->new_lines_[i] += 1;
            // }
            *new_file_size += 1;
            update_lines(line_buf, 1);
            getyx(*edit_window, y, x);
            update_edit_window(char_buf, line_buf, new_file_size, *line_num_win,
                               *edit_window);
            wmove(*edit_window, y, x + 1);
            char_buf->mod_size_ = 0;
            wrefresh(*edit_window);
            break;
    }
}

int get_line_length(LINE_BUFFER *lbuf, int line_index)
{
    int actual_index;

    // if (line_index < (lbuf->ccur_ - lbuf->new_lines_)) {
    //     actual_index = line_index;
    // } else {
    actual_index = line_index + lbuf->gap_size_;
    // }

    if (line_index == 0)
    {
        return lbuf->new_lines_[actual_index] + 1;
    }

    int prev_actual_index;
    // if ((line_index - 1) < (lbuf->ccur_ - lbuf->new_lines_))
    // {
    //     prev_actual_index = line_index - 1;
    // } else {
    prev_actual_index = (line_index - 1) + lbuf->gap_size_;
    // }

    return lbuf->new_lines_[actual_index] - lbuf->new_lines_[prev_actual_index];
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
    lbuf->ccur_ = lbuf->ccur_ + lbuf->gap_size_;
}

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
