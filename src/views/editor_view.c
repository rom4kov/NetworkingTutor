// #include <pcre.h>
#include "../models/models.h"

#include <curses.h>
#include <ncurses.h>
#include <pcre.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <string.h>

void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                         int *scroll_offset, int lines_to_print)
{
    // if (lines_to_print < tbuf->num_of_lines && tbuf->num_of_lines)
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
}

void compile_patterns(pcre2_code **re, int p_codes_num, char **pattern_str)
{
    int errcode;
    PCRE2_SIZE erroffset;

    for (int i = 0; i < p_codes_num; i++)
    {
        PCRE2_SPTR pattern = (PCRE2_SPTR)pattern_str[i];
        re[i] = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UCP,
                              &errcode, &erroffset, NULL);
        if (!re[i])
            return;
    }
}

void print_matches(pcre2_code *re, int i, size_t subject_length, char *line_buf,
                   int group, WINDOW **edit_window, int color)
{
    PCRE2_SPTR subject = (PCRE2_SPTR)line_buf;
    pcre2_match_data *md = pcre2_match_data_create_from_pattern(re, NULL);
    if (!md)
        return;

    int rc = pcre2_match(re, subject, subject_length, 0, 0, md, NULL);
    int line_len = 0;

    while (rc > 0)
    {
        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(md);
        PCRE2_SIZE start = ovector[group];
        PCRE2_SIZE end = ovector[group + 1];

        wattron(*edit_window, COLOR_PAIR(color));
        for (PCRE2_SIZE k = start; k < end; k++)
            mvwprintw(*edit_window, i, k, "%c", line_buf[k]);
        wattroff(*edit_window, COLOR_PAIR(color));

        line_len = (int)end > line_len ? (int)end : line_len + 1;

        rc = pcre2_match(re, subject, subject_length, line_len, 0, md, NULL);
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

    int pattern_num = 7;
    pcre2_code *re[pattern_num];
    char *patterns[] = {
        ".",
        "\\b(void|int|char|return|for|while|if|else|break|continue)\\b",
        "#(include|define)|=|\\+|\\-|\\*|\\&|<|>",
        "(\".*\"|<.*\\.h>)",
        "([a-z0-9_]*)\\(.*\\)",
        "\\b\\d*\\b",
        "//.*",
    };
    int colors[] = {1, 8, 6, 4, 7, 9, 10};

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < lines_to_print; i++)
    {
        if (current_line != NULL)
        {
            size_t subj_len = strlen(current_line->buf_);

            for (int j = 0; j < pattern_num; j++)
            {
                print_matches(re[j], i, subj_len, current_line->buf_,
                              j == 4 ? 2 : 0, edit_window, colors[j]);
            }

            current_line = current_line->next;
        }
    }

    for (int i = 0; i < pattern_num; i++)
        pcre2_code_free(re[i]);

    update_line_numbers(tbuf, line_num_win, scroll_offset, lines_to_print);
}

void print_line(char *line_buf, int line_num, WINDOW **edit_window)
{
    int pattern_num = 7;
    pcre2_code *re[pattern_num];
    char *patterns[] = {
        ".",
        "\\b(void|int|char|return|for|while|if|else|break|continue)\\b",
        "#(include|define)|=|\\+|\\-|\\*|\\&",
        "(\".*\"|<.*\\.h>)",
        "([a-z0-9_]*)\\(.*\\)",
        "\\d",
        "//.*",
    };
    int colors[] = {1, 8, 6, 4, 7, 9, 10};
    size_t subj_len = strlen(line_buf);

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        print_matches(re[i], line_num, subj_len, line_buf, i == 4 ? 2 : 0,
                      edit_window, colors[i]);
    }

    for (int i = 0; i < pattern_num; i++)
        pcre2_code_free(re[i]);
}

char *get_file_icon(pcre2_code *re, int subj_len, char *filename, char **icon)
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

ICON print_file_icon(char *filename)
{
    int pattern_num = 10;
    pcre2_code *re[pattern_num];

    char *patterns[] = {
        ".*\\.c\\b", ".*\\.h",  ".*\\.txt", ".*\\.py",  ".*\\.js",
        ".*\\.ts",   ".*\\.rs", ".*\\.db",  ".*\\.git", "Makefile",
    };
    char *icons[] = {
        " ", " ", " ", " ", " ", " ", " ", " ", " ", ""
    };

    ICON matched_icon = {.icon = NULL, .color = 2};

    int colors[] = {7, 8, 4, 5, 5, 7, 9, 2, 9, 1};
    size_t subj_len = strlen(filename);

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        matched_icon.icon = get_file_icon(re[i], subj_len, filename, &icons[i]);

        if (matched_icon.icon != NULL)
        {
            matched_icon.color = colors[i];
            return matched_icon;
        }
    }
    return matched_icon;
}
