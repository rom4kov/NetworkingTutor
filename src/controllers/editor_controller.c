#include "../models/models.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

void write_buffer_to_file(CHAR_BUFFER *cbuf, FILE *file, int file_size,
                          WINDOW *edit_window, char *ccur, char *cend,
                          LINE_BUFFER *lbuf)
{
    for (int i = 0; i < lbuf->size_; i++)
    {
        mvwprintw(edit_window, LINES - 28 + i, 6, "%i", lbuf->line_size_[i]);
    }

    mvwprintw(edit_window, LINES - 16, 52, "After:");
    mvwprintw(edit_window, LINES - 15, 52, "Mod size: %i", cbuf->mod_size_);
    mvwprintw(edit_window, LINES - 14, 52, "File pointer: %li", ftell(file));
    mvwprintw(edit_window, LINES - 13, 52, "First byte of buffer: %p",
              cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 52, "ccur: %p", ccur);
    mvwprintw(edit_window, LINES - 11, 52, "cend: %p", cend);

    int first_buf_len = ccur - cbuf->buf_;
    mvwprintw(edit_window, LINES - 10, 52, "First buffer length: %i",
              first_buf_len);
    int second_buf_len = &cbuf->buf_[file_size] - cend;
    mvwprintw(edit_window, LINES - 9, 52, "Second buffer length: %i",
              second_buf_len);

    fwrite(cbuf->buf_, sizeof(char), first_buf_len, file);
    fwrite(cend, sizeof(char), second_buf_len, file);

    wrefresh(edit_window);
}

void handle_editor_input(int ch, WINDOW *edit_window, int y, int x,
                         CHAR_BUFFER *char_buf, LINE_BUFFER *line_buf,
                         int gap_size, bool *editor_mode, FILE *file,
                         int file_size)
{
    switch (ch)
    {
        case KEY_RIGHT:
            char_buf->ccur_++;
            char_buf->cend_++;
            char_buf->mod_size_++;
            getyx(edit_window, y, x);
            wmove(edit_window, y, x + 1);
            wrefresh(edit_window);
            break;
        case KEY_LEFT:
            char_buf->ccur_--;
            char_buf->cend_--;
            char_buf->mod_size_--;
            getyx(edit_window, y, x);
            wmove(edit_window, y, x - 1);
            wrefresh(edit_window);
            break;
        case KEY_DOWN:
            char_buf->ccur_ += line_buf->line_size_[line_buf->ccur_];
            char_buf->cend_ += line_buf->line_size_[line_buf->ccur_];
            char_buf->mod_size_ += line_buf->line_size_[line_buf->ccur_];
            line_buf->ccur_++;
            getyx(edit_window, y, x);
            wmove(edit_window, y + 1, x);
            wrefresh(edit_window);
            break;
        case KEY_UP:
            char_buf->ccur_ -= line_buf->line_size_[line_buf->ccur_ - 1];
            char_buf->cend_ -= line_buf->line_size_[line_buf->ccur_ - 1];
            char_buf->mod_size_ -= line_buf->line_size_[line_buf->ccur_];
            line_buf->ccur_--;
            getyx(edit_window, y, x);
            wmove(edit_window, y - 1, x);
            wrefresh(edit_window);
            break;
        case KEY_BACKSPACE:
            if (char_buf->mod_size_ > 0)
                memcpy(char_buf->ccur_ - char_buf->mod_size_,
                       char_buf->cend_ - char_buf->mod_size_,
                       char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memcpy(char_buf->cend_, char_buf->ccur_, char_buf->mod_size_);
            char_buf->mod_size_ = 0;
            char_buf->ccur_--;
            // gap_size++;
            line_buf->line_size_[line_buf->ccur_] =
                line_buf->line_size_[line_buf->ccur_] + 1;
            getyx(edit_window, y, x);
            if (x == 0)
            {
                wmove(edit_window, y - 1, x);
                wrefresh(edit_window);
                break;
            }
            mvwprintw(edit_window, y, x - 1, " ");
            wmove(edit_window, y, x - 1);
            wrefresh(edit_window);
            break;
        case 10:
            *char_buf->ccur_ = '\n';
            gap_size--;
            getyx(edit_window, y, x);
            wprintw(edit_window, "%c", '\n');
            wmove(edit_window, y + 1, 0);
            wrefresh(edit_window);
            break;
        case KEY_F(1):
            curs_set(0);
            wrefresh(edit_window);
            *editor_mode = false;
            break;
        case KEY_F(10):
            mvwprintw(edit_window, LINES - 9, 22, "test");
            write_buffer_to_file(char_buf, file, file_size, edit_window,
                                 char_buf->ccur_, char_buf->cend_, line_buf);
            break;
        default:
            if (char_buf->mod_size_ > 0)
                memcpy(char_buf->ccur_ - char_buf->mod_size_,
                       char_buf->cend_ - char_buf->mod_size_,
                       char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memcpy(char_buf->cend_, char_buf->ccur_, char_buf->mod_size_);
            *char_buf->ccur_ = ch;
            char_buf->ccur_++;
            gap_size--;
            line_buf->line_size_[line_buf->ccur_] =
                line_buf->line_size_[line_buf->ccur_] + 1;
            wprintw(edit_window, "%c", ch);
            // wmove(edit_window, y, x + 1);
            // mvwprintw(edit_window, LINES - 15, 2, "Mod size: %i", char_buf->mod_size_);
            char_buf->mod_size_ = 0;
            wrefresh(edit_window);
            break;
    }
}

// void update_edit_window(CHAR_BUFFER *cbuf, LINE_BUFFER *lbuf,
//                         WINDOW **line_num_win, WINDOW **edit_window,
//                         int gap_size)
// {
//     int i, j, k;
//     i = j = k = 0;
//
//     for (i = 0; i < lbuf->size_; i++)
//     {
//         if (i < 9)
//             mvwprintw(*line_num_win, i + 1, 1, "%i", i + 1);
//         else
//             mvwprintw(*line_num_win, i + 1, 0, "%i", i + 1);
//         for (j = 0; j < lbuf->line_size_[i]; j++)
//         {
//             mvwprintw(*edit_window, i, j, "%c", cbuf->buf_[k + gap_size]);
//             k++;
//         }
//     }
// }
