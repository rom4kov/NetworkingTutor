// #include <pcre.h>
#include <pcre.h>
#define PCRE2_CODE_UNIT_WIDTH 8

#include "../models/models.h"
#include <ncurses.h>
#include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <regex.h>
#include <pcre2.h>

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
            return; // Compilation failed
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
    //
    // int errcode;
    // PCRE2_SIZE erroffset;

    int pattern_num = 6;
    pcre2_code *re[pattern_num];
    char *patterns[] = {".",
                        "\\b(void|int|char|return)\\b",
                        "#(include|define)",
                        "(\".*\"|<.*\\.h>)",
                        "([a-z0-9_]*)\\(.*\\)",
                        "\\d"};
    int colors[] = {2, 8, 6, 4, 7, 3};

    compile_patterns(re, pattern_num, patterns);
    // pcre2_code *re5;
    // PCRE2_SPTR pattern5 = (PCRE2_SPTR) ".";
    // re5 = pcre2_compile(pattern5, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
    //                     &erroffset, NULL);
    // if (!re5)
    //     return; // Compilation failed
    //
    // pcre2_code *re;
    // PCRE2_SPTR pattern = (PCRE2_SPTR) "\\b(void|int|char|return)\\b";
    // re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
    //                    &erroffset, NULL);
    // if (!re)
    //     return; // Compilation failed
    //
    // pcre2_code *re2;
    // PCRE2_SPTR pattern2 = (PCRE2_SPTR) "#(include|define)";
    // re2 = pcre2_compile(pattern2, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
    //                     &erroffset, NULL);
    // if (!re)
    //     return; // Compilation failed
    //
    // pcre2_code *re3;
    // PCRE2_SPTR pattern3 = (PCRE2_SPTR) "(\".*\"|<.*\\.h>)";
    // re3 = pcre2_compile(pattern3, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
    //                     &erroffset, NULL);
    // if (!re3)
    //     return; // Compilation failed
    //
    // pcre2_code *re4;
    // PCRE2_SPTR pattern4 = (PCRE2_SPTR) "([a-z0-9_]*)\\(.*\\)";
    // re4 = pcre2_compile(pattern4, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
    //                     &erroffset, NULL);
    // if (!re4)
    //     return; // Compilation failed
    //
    // pcre2_code *re6;
    // PCRE2_SPTR pattern6 = (PCRE2_SPTR) "\\d";
    // re6 = pcre2_compile(pattern6, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
    //                     &erroffset, NULL);
    // if (!re6)
    //     return; // Compilation failed

    for (int i = 0; i < tbuf->num_of_lines; i++)
    {
        size_t subj_len = strlen(current_line->buf_);

        for (int j = 0; j < pattern_num; j++)
        {
            print_matches(re[j], i, subj_len, current_line->buf_,
                          j == 4 ? 2 : 0, edit_window, colors[j]);
        }
        // print_matches(re[1], i, subj_len, current_line->buf_, 0, edit_window,
        // 8); print_matches(re[2], i, subj_len, current_line->buf_, 0,
        // edit_window, 6); print_matches(re[3], i, subj_len,
        // current_line->buf_, 0, edit_window, 4); print_matches(re[4], i,
        // subj_len, current_line->buf_, 2, edit_window, 7);
        // print_matches(re[5], i, subj_len, current_line->buf_, 0, edit_window,
        // 3);

        current_line = current_line->next;
    }

    for (int i = 0; i < pattern_num; i++)
        pcre2_code_free(re[i]);
    update_line_numbers(tbuf, line_num_win);
}
