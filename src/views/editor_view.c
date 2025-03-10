#include "../models/models.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

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
    werase(*line_num_win);
    werase(*edit_window);
    int word_len = 0;
    int line_len = 0;
    char *word;

    LINE *current_line = tbuf->first_line;

    for (int i = 0; i < tbuf->num_of_lines; i++)
    {
        for (int k = 0; k < current_line->length; k++)
        {
            if (current_line->buf_[k] == ' ' || current_line->buf_[k] == '\n' ||
                current_line->buf_[k] == '(' || current_line->buf_[k] == ')')
            {
                word_len++;
                word = malloc(sizeof(word_len));
                strncpy(word, &current_line->buf_[line_len], word_len);
                word[word_len - 1] = '\0';

                // mvwprintw(*edit_window, LINES + i - 25, line_len, "%s",
                // word);
                if (strcmp(word, "#include") == 0)
                {
                    wattron(*edit_window, COLOR_PAIR(6));
                }
                if (strcmp(word, "<stdio.h>") == 0)
                {
                    wattron(*edit_window, COLOR_PAIR(4));
                }
                if (strcmp(word, "int") == 0 || strcmp(word, "void") == 0 ||
                    strcmp(word, "return") == 0)
                {
                    wattron(*edit_window, COLOR_PAIR(8));
                }

                for (int j = 0; j < word_len; j++)
                {
                    mvwprintw(*edit_window, i, line_len + j, "%c",
                              current_line->buf_[line_len + j]);
                }

                if (strcmp(word, "#include") == 0)
                {
                    wattroff(*edit_window, COLOR_PAIR(6));
                }
                if (strcmp(word, "<stdio.h>") == 0)
                {
                    wattroff(*edit_window, COLOR_PAIR(4));
                }
                if (strcmp(word, "int") == 0 || strcmp(word, "void") == 0 ||
                    strcmp(word, "return") == 0)
                {
                    wattroff(*edit_window, COLOR_PAIR(8));
                }
                line_len += word_len;
                word_len = 0;
                free(word);
            }
            else
            {
                word_len++;
            }
        }
        line_len = 0;
        word_len = 0;
        current_line = current_line->next;
    }
    update_line_numbers(tbuf, line_num_win);
}
