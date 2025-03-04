#define _XOPEN_SOURCE 500

#include "../core/core.h"
#include "../models/models.h"
#include "utils.h"
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
    mvwprintw(edit_window, LINES - 8, 52, "Num of lines: %i", lbuf->size_);

    fwrite(cbuf->buf_, sizeof(char), first_buf_len, file);
    fwrite(cbuf->cend_, sizeof(char), second_buf_len, file);

    rewind(file);

    // int first_lbuf_len = lbuf->ccur_ - lbuf->new_lines_;
    // int second_lbuf_len =
    //     &lbuf->new_lines_[lbuf->size_ + lbuf->gap_size_] - lbuf->cend_;
    FILE *lines_file = fopen("../lines.txt", "r+");
    fwrite(lbuf->new_lines_, sizeof(int), lbuf->size_ + lbuf->gap_size_,
           lines_file);
    // fwrite(lbuf->cend_, sizeof(int), second_lbuf_len, lines_file);

    // int chars_diff = *new_file_size - file_size;
    mvwprintw(edit_window, LINES - 20, 52, "file_size: %i", file_size);
    mvwprintw(edit_window, LINES - 21, 52, "new_file_size: %i", *new_file_size);

    wrefresh(edit_window);
}

void handle_editor_input(int ch, WINDOW **line_num_win, WINDOW **edit_window,
                         int y, int x, CHAR_BUFFER *char_buf,
                         LINE_BUFFER *line_buf, bool *editor_mode, FILE *file,
                         int file_size, int *new_file_size)
{
    getyx(*edit_window, y, x);
    int curr_line = y;
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

    // int ccur_change_kdown;
    // int ccur_change_kup;

    switch (ch)
    {
        case KEY_RIGHT:
            if (x + 1 < curr_line_len)
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
            if ((y + 1) < line_buf->size_)
            {
                // if (next_line_len - 1 > x)
                //     ccur_change_kdown = curr_line_len;
                // else
                // {
                //     ccur_change_kdown =
                //         line_buf
                //             ->new_lines_[curr_line + 1 + line_buf->gap_size_]
                //             -
                //         (char_buf->ccur_ - char_buf->buf_) -
                //         (next_line_len < 2 ? 0 : 1);
                // }
                int ccur_change_kdown = calculate_cursor_offset(
                    line_buf, char_buf, y, x, prev_line_len, curr_line_len,
                    next_line_len, 1);
                char_buf->ccur_ += ccur_change_kdown;
                char_buf->cend_ += ccur_change_kdown;
                char_buf->mod_size_ += ccur_change_kdown;
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
                mvwprintw(*edit_window, LINES - 9, 90, "currl: %i",
                          curr_line + 1);
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
                // if (prev_line_len - 1 > x)
                //     ccur_change_kup = prev_line_len;
                // else
                // {
                //     ccur_change_kup =
                //         (char_buf->ccur_ - char_buf->buf_) -
                //         line_buf
                //             ->new_lines_[curr_line - 1 + line_buf->gap_size_]
                //             +
                //         (prev_line_len < 2 ? 0 : 1);
                // }
                int ccur_change_kup = calculate_cursor_offset(
                    line_buf, char_buf, y, x, prev_line_len, curr_line_len,
                    next_line_len, -1);
                char_buf->ccur_ -= ccur_change_kup;
                char_buf->cend_ -= ccur_change_kup;
                char_buf->mod_size_ -= ccur_change_kup;
                // line_buf->ccur_--;
                // line_buf->cend_--;
                // line_buf->mod_size_++;
                mvwprintw(*edit_window, LINES - 11, 50, "             ");
                mvwprintw(
                    *edit_window, LINES - 11, 50, "lbnl: %li",
                    line_buf->ccur_ - line_buf->new_lines_);
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
                mvwprintw(*edit_window, LINES - 9, 90, "currl: %i",
                          curr_line);
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

            *char_buf->ccur_ = '\n';
            char_buf->gap_size_ -= 1;

            move_gap_to_line(line_buf, curr_line);
            insert_new_line(line_buf, char_buf);

            *new_file_size += 1;
            update_lines(line_buf, 1);
            char_buf->ccur_++;
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
            update_edit_window(char_buf, line_buf, new_file_size, *line_num_win,
                               *edit_window);
            wmove(*edit_window, y, x + 1);
            char_buf->mod_size_ = 0;
            wrefresh(*edit_window);
            break;
    }
}
