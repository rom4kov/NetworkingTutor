// #include <pcre.h>
#include <pcre.h>
#define PCRE2_CODE_UNIT_WIDTH 8

#include "../models/models.h"
#include <ncurses.h>
#include <pcre2.h>
#include <string.h>

void update_line_numbers(TEXT_BUFFER *tbuf, WINDOW **line_num_win)
{
    int i;
    for (i = 0; i < tbuf->num_of_lines; i++)
    {
        wattron(*line_num_win, COLOR_PAIR(10));
        if (i == tbuf->curr_line_nr)
        {
            wattroff(*line_num_win, COLOR_PAIR(10));
            wattron(*line_num_win, COLOR_PAIR(1));
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
                  WINDOW **line_num_win)
{
    werase(*line_num_win);
    werase(*edit_window);

    LINE *current_line = tbuf->first_line;

    int pattern_num = 6;
    pcre2_code *re[pattern_num];
    char *patterns[] = {
        ".",
        "\\b(void|int|char|return|for|while|if|else|break|continue)\\b",
        "#(include|define)|=|\\+|\\-|\\*|\\&",
        "(\".*\"|<.*\\.h>)",
        "([a-z0-9_]*)\\(.*\\)",
        "\\d"};
    int colors[] = {1, 8, 6, 4, 7, 9};

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < tbuf->num_of_lines; i++)
    {
        size_t subj_len = strlen(current_line->buf_);

        for (int j = 0; j < pattern_num; j++)
        {
            print_matches(re[j], i, subj_len, current_line->buf_,
                          j == 4 ? 2 : 0, edit_window, colors[j]);
        }

        current_line = current_line->next;
    }

    for (int i = 0; i < pattern_num; i++)
        pcre2_code_free(re[i]);

    update_line_numbers(tbuf, line_num_win);
}

void print_line(char *line_buf, int line_num, WINDOW **edit_window)
{
    int pattern_num = 6;
    pcre2_code *re[pattern_num];
    char *patterns[] = {
        ".",
        "\\b(void|int|char|return|for|while|if|else|break|continue)\\b",
        "#(include|define)|=|\\+|\\-|\\*|\\&",
        "(\".*\"|<.*\\.h>)",
        "([a-z0-9_]*)\\(.*\\)",
        "\\d"};
    int colors[] = {1, 8, 6, 4, 7, 9};
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
