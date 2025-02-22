#include "../models/models.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

void write_buffer_to_file(CHAR_BUFFER *cbuf, FILE *file, int file_size, WINDOW *edit_window)
{
    mvwprintw(edit_window, LINES - 10, 22, "test2");
    // int i;
    // for (i = 0; i < lbuf->size_; i++)
    // {
        // int chars_printed = 0;
    fwrite(cbuf->buf_, sizeof(char), file_size, file);
    //     chars_printed += lbuf->line_size_[i];
    // }
}

void handle_editor_input(int ch, WINDOW *edit_window, int y, int x,
                         CHAR_BUFFER *char_buf, LINE_BUFFER *line_buf,
                         int gap_size, bool *editor_mode, FILE *file, int file_size)
{
    switch (ch)
    {
        case KEY_RIGHT:
            char_buf->ccur_++;
            char_buf->cend_++;
            getyx(edit_window, y, x);
            wmove(edit_window, y, x + 1);
            wrefresh(edit_window);
            break;
        case KEY_LEFT:
            char_buf->last_mod_ = char_buf->ccur_;
            char_buf->ccur_--;
            char_buf->cend_--;
            getyx(edit_window, y, x);
            wmove(edit_window, y, x - 1);
            wrefresh(edit_window);
            break;
        case KEY_DOWN:
            char_buf->ccur_ += line_buf->line_size_[line_buf->ccur_];
            char_buf->cend_ += line_buf->line_size_[line_buf->ccur_];
            line_buf->ccur_++;
            getyx(edit_window, y, x);
            wmove(edit_window, y + 1, x);
            wrefresh(edit_window);
            break;
        case KEY_UP:
            char_buf->ccur_ -= line_buf->line_size_[line_buf->ccur_ - 1];
            char_buf->cend_ -= line_buf->line_size_[line_buf->ccur_ - 1];
            line_buf->ccur_--;
            getyx(edit_window, y, x);
            wmove(edit_window, y - 1, x);
            wrefresh(edit_window);
            break;
        case KEY_BACKSPACE:
            char_buf->ccur_--;
            gap_size++;
            line_buf->line_size_[line_buf->ccur_]--;
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
            write_buffer_to_file(char_buf, file, file_size, edit_window);
            break;
        default:
            if (char_buf->ccur_ > char_buf->last_mod_)
                memcpy(char_buf->ccur_, char_buf->cend_,
                       char_buf->ccur_ - char_buf->last_mod_);
            else
                memcpy(char_buf->ccur_, char_buf->cend_,
                       char_buf->last_mod_ - char_buf->ccur_);
            char_buf->last_mod_ = char_buf->ccur_;
            *char_buf->ccur_ = ch;
            char_buf->ccur_++;
            gap_size--;
            line_buf->line_size_[line_buf->ccur_]++;
            wprintw(edit_window, "%c", ch);
            // wmove(edit_window, y, x + 1);
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
