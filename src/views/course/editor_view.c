#include "../../data/data_access_layer.h"
#include "../../models/models.h"

#include <curses.h>
#include <ncurses.h>
#include <pcre.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDITOR_WIDTH ((WU * 7 + 4) - (WU + WU / 2))

void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         int *scroll_offset, int lines_to_print)
{
    for (int i = 0; i < lines_to_print; i++)
    {
        wattron(*line_num_win, COLOR_PAIR(10));
        if (i + *scroll_offset == tbuf->curr_line_nr)
        {
            wattroff(*line_num_win, COLOR_PAIR(10));
            wattron(*line_num_win, COLOR_PAIR(1));
        }

        if (i + *scroll_offset < 9)
        {
            mvwprintw(*line_num_win, i, 2, "%i", i + *scroll_offset + 1);
        }
        else if (i + *scroll_offset < 99)
        {
            mvwprintw(*line_num_win, i, 1, "%i", i + *scroll_offset + 1);
        }
        else
        {
            mvwprintw(*line_num_win, i, 0, "%i", i + *scroll_offset + 1);
        }

        if (i + *scroll_offset == tbuf->curr_line_nr)
        {
            wattroff(*line_num_win, COLOR_PAIR(1));
            wattron(*line_num_win, COLOR_PAIR(10));
        }
    }
    wattroff(*line_num_win, COLOR_PAIR(10));

    if (lines_to_print < LINES - 5)
    {
        for (int i = lines_to_print; i < (LINES - 7); i++)
        {
            wattron(*line_num_win, COLOR_PAIR(11));
            mvwprintw(*line_num_win, i, 1, "%s", i % 2 == 0 ? "`" : " ");
            wattroff(*line_num_win, COLOR_PAIR(11));
        }
    }
}

void compile_patterns(pcre2_code **re, int p_codes_num, char **pattern_str)
{
    int errcode;
    PCRE2_SIZE erroffset;
    PCRE2_UCHAR buffer[64];

    for (int i = 0; i < p_codes_num; i++)
    {
        PCRE2_SPTR pattern = (PCRE2_SPTR)pattern_str[i];
        re[i] = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UCP,
                              &errcode, &erroffset, NULL);
        pcre2_get_error_message(errcode, buffer, 30);
        if (!re[i])
            return;
    }
}

void print_matches(pcre2_code **re, int line_num, int j, size_t subject_length,
                   LINE *current_line, int group, WINDOW **edit_window,
                   int color)
{
    PCRE2_SPTR subject = (PCRE2_SPTR)current_line->buf_;
    pcre2_match_data *md = pcre2_match_data_create_from_pattern(re[j], NULL);
    if (!md)
        return;

    int rc = pcre2_match(re[j], subject, subject_length, 0, 0, md, NULL);
    if (j == 4 && rc > 0)
    {
        char *extended_buf = calloc(500, sizeof(char));
        LINE *temp_line = initialize_line();
        temp_line = current_line;

        int i = 0;
        int paren_depth = 0;
        bool closing_bracket_reached = false;
        strcat(extended_buf, temp_line->buf_);
        while (temp_line != NULL && !closing_bracket_reached)
        {
            if (temp_line->buf_[i] == '(')
            {
                paren_depth++;
            }
            else if (temp_line->buf_[i] == '\0')
            {
                temp_line = temp_line->next;
                if (temp_line != NULL)
                    strcat(extended_buf, temp_line->buf_);
                i = 0;
                continue;
            }
            else if (temp_line->buf_[i] == ')')
            {
                paren_depth--;
                if (paren_depth == 0)
                    closing_bracket_reached = true;
            }
            i++;
        }
        if (!closing_bracket_reached)
            return;
        extended_buf[strlen(extended_buf)] = '\0';

        subject_length = strlen(extended_buf);

        subject = (PCRE2_SPTR)extended_buf;
        md = pcre2_match_data_create_from_pattern(re[3], NULL);
        if (!md)
            return;
        rc = pcre2_match(re[3], subject, subject_length, 0, 0, md, NULL);
        free(extended_buf);
    }
    int line_len = 0;

    while (rc > 0)
    {
        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(md);
        PCRE2_SIZE start = ovector[group];
        PCRE2_SIZE end = ovector[group + 1];

        wattron(*edit_window, COLOR_PAIR(color));
        for (PCRE2_SIZE k = start; k < end; k++)
        {
            mvwprintw(*edit_window, line_num, k, "%c", current_line->buf_[k]);
        }
        wattroff(*edit_window, COLOR_PAIR(color));

        line_len = (int)end > line_len ? (int)end : line_len + 1;

        rc = pcre2_match(re[j == 4 ? 3 : j], subject, subject_length, line_len,
                         0, md, NULL);
    }

    pcre2_match_data_free(md);
}

void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win, int *scroll_offset, int lines_to_print)
{
    werase(*line_num_win);
    werase(*edit_window);

    LINE *current_line = tbuf->first_line;
    for (int i = 0; i < *scroll_offset; i++)
    {
        current_line = current_line->next;
    }

    int pattern_num = 10;

    pcre2_code *re[pattern_num];

    char *patterns[] = {
        ".",
        "\\b(void|int|char|return|for|while|if|else|break|continue|bool|switch|struct|case|default)\\b",
        "#(include|define)|NULL|=|\\+|\\-|\\*|\\&|<|>|;",
        "(?s)\\b([a-z0-9_]*)\\(.*\\)",
        "([a-z0-9_]*)\\(.*",
        "\\b(?:\\d+(\\.\\d+)?|true|false)\\b",
        "(\".*\"|<.*\\.h>)",
        "(?<=\\bstruct )\\w+",
        "(?<=(->|\\.))\\w+(?!(>|\"))",
        "(//.*|/\\*.*\\*/)",
    };

    int colors[] = {1, 8, 6, 7, 7, 9, 4, 14, 15, 10};

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < lines_to_print; i++)
    {
        if (current_line != NULL)
        {
            size_t subj_len = strlen(current_line->buf_);

            for (int j = 0; j < pattern_num; j++)
            {
                print_matches(re, i, j, subj_len, current_line,
                              (j == 3 || j == 4) ? 2 : 0, edit_window,
                              colors[j]);
            }

            current_line = current_line->next;
        }
    }

    for (int i = 0; i < pattern_num; i++)
        pcre2_code_free(re[i]);

    update_line_numbers(tbuf, line_num_win, scroll_offset, lines_to_print);
}

void print_line(LINE *current_line, int line_num, WINDOW **edit_window)
{
    int pattern_num = 10;
    pcre2_code *re[pattern_num];
    char *patterns[] = {
        ".",
        "\\b(void|int|char|return|for|while|if|else|break|continue|bool|switch|"
        "struct|"
        "case|default)\\b",
        "#(include|define)|NULL|=|\\+|\\-|\\*|\\&|<|>|;",
        "(?s)\\b([a-z0-9_]*)\\(.*\\)",
        "([a-z0-9_]*)\\(.*",
        "\\b(?:\\d+(\\.\\d+)?|true|false)\\b",
        "(\".*\"|<.*\\.h>)",
        "(?<=\\bstruct )\\w+",
        "(?<=(->|\\.))\\w+(?!(>|\"))",
        "(//.*|/\\*.*\\*/)",
    };
    int colors[] = {1, 8, 6, 7, 7, 9, 4, 14, 15, 10};
    size_t subj_len = strlen(current_line->buf_);

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        print_matches(re, line_num, i, subj_len, current_line,
                      (i == 3 || i == 4) ? 2 : 0, edit_window, colors[i]);
    }

    for (int i = 0; i < pattern_num; i++)
        pcre2_code_free(re[i]);
}

char *match_file_icon(pcre2_code *re, int subj_len, char *filename, char **icon)
{
    PCRE2_SPTR subject = (PCRE2_SPTR)filename;
    pcre2_match_data *md = pcre2_match_data_create_from_pattern(re, NULL);
    if (!md)
        return NULL;

    int rc = pcre2_match(re, subject, subj_len, 0, 0, md, NULL);

    if (rc > 0)
    {
        return *icon;
    }

    pcre2_match_data_free(md);

    return NULL;
}

ICON get_file_icon(char *filename)
{
    int pattern_num = 10;
    pcre2_code *re[pattern_num];

    char *patterns[] = {
        ".*\\.c\\b", ".*\\.h",  ".*\\.txt", ".*\\.py",  ".*\\.js",
        ".*\\.ts",   ".*\\.rs", ".*\\.db",  ".*\\.git", "Makefile",
    };
    char *icons[] = {" ", " ", " ", " ", " ",
                     " ", " ", " ", " ", ""};

    ICON matched_icon = {.icon = NULL, .color = 2};

    int colors[] = {7, 8, 4, 5, 5, 7, 9, 2, 9, 1};
    size_t subj_len = strlen(filename);

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        matched_icon.icon =
            match_file_icon(re[i], subj_len, filename, &icons[i]);

        if (matched_icon.icon != NULL)
        {
            matched_icon.color = colors[i];
            return matched_icon;
        }
    }
    return matched_icon;
}

void print_editor_meta_data(APP_CONTEXT *ctx)
{
    mvwprintw(ctx->course_windows[2], LINES - 5, EDITOR_WIDTH, "%i",
              ctx->t_buffer->curr_line_nr);
}
