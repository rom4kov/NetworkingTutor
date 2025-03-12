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

    int errcode;
    PCRE2_SIZE erroffset;

    pcre2_code *re;
    PCRE2_SPTR pattern = (PCRE2_SPTR) "\\b(void|int|char|return)\\b";
    re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
                       &erroffset, NULL);
    if (!re)
        return; // Compilation failed

    pcre2_code *re2;
    PCRE2_SPTR pattern2 = (PCRE2_SPTR) "#(include|define)";
    re2 = pcre2_compile(pattern2, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
                        &erroffset, NULL);
    if (!re)
        return; // Compilation failed

    pcre2_code *re3;
    PCRE2_SPTR pattern3 = (PCRE2_SPTR) "(\".*\"|<.*\\.h>)";
    re3 = pcre2_compile(pattern3, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
                        &erroffset, NULL);
    if (!re3)
        return; // Compilation failed

    pcre2_code *re4;
    PCRE2_SPTR pattern4 = (PCRE2_SPTR) "([a-z0-9_]*)\\(.*\\)";
    re4 = pcre2_compile(pattern4, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
                        &erroffset, NULL);
    if (!re4)
        return; // Compilation failed

    pcre2_code *re5;
    PCRE2_SPTR pattern5 = (PCRE2_SPTR) ".";
    re5 = pcre2_compile(pattern5, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
                        &erroffset, NULL);
    if (!re5)
        return; // Compilation failed

    pcre2_code *re6;
    PCRE2_SPTR pattern6 = (PCRE2_SPTR) "\\d";
    re6 = pcre2_compile(pattern6, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode,
                        &erroffset, NULL);
    if (!re6)
        return; // Compilation failed
    

    // rc = pcre2_match(re, subject, subject_length, 0, 0, match_data, NULL);

    for (int i = 0; i < tbuf->num_of_lines; i++)
    {
        // PCRE2_SPTR subject = (PCRE2_SPTR)current_line->buf_;
        // if (subject == NULL)
        // { // Check for NULL buffer
        //     current_line = current_line->next;
        //     continue;
        // }

        size_t subj_len = strlen(current_line->buf_);

        print_matches(re5, i, subj_len, current_line->buf_, 0, edit_window, 2);
        print_matches(re, i, subj_len, current_line->buf_, 0, edit_window, 8);
        print_matches(re2, i, subj_len, current_line->buf_, 0, edit_window, 6);
        print_matches(re3, i, subj_len, current_line->buf_, 0, edit_window, 4);
        print_matches(re4, i, subj_len, current_line->buf_, 2, edit_window, 7);
        print_matches(re6, i, subj_len, current_line->buf_, 0, edit_window, 3);

        // match_data = pcre2_match_data_create_from_pattern(re, NULL);
        // if (!match_data)
        // {
        //     pcre2_code_free(re);
        //     return;
        // }
        //
        // match_data2 = pcre2_match_data_create_from_pattern(re, NULL);
        // if (!match_data2)
        // {
        //     pcre2_code_free(re);
        //     return;
        // }
        //
        // line_len = 0;
        // rc = pcre2_match(re, subject, subject_length, 0, 0, match_data,
        // NULL); rc2 =
        //     pcre2_match(re2, subject, subject_length, 0, 0, match_data2,
        //     NULL);

        // while (rc > 0)
        // {
        //     PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
        //     PCRE2_SIZE start = ovector[0];
        //     PCRE2_SIZE end = ovector[1];
        //
        //     wattron(*edit_window, COLOR_PAIR(8));
        //     for (PCRE2_SIZE k = start; k < end; k++)
        //         mvwprintw(*edit_window, i, k, "%c", current_line->buf_[k]);
        //     wattroff(*edit_window, COLOR_PAIR(8));
        //
        //     line_len += end;
        //     rc = pcre2_match(re, subject, subject_length, line_len, 0,
        //                      match_data, NULL);
        // }
        //
        // while (rc2 > 0)
        // {
        //     PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data2);
        //     PCRE2_SIZE start = ovector[0];
        //     PCRE2_SIZE end = ovector[1];
        //
        //     wattron(*edit_window, COLOR_PAIR(6));
        //     for (PCRE2_SIZE k = start; k < end; k++)
        //         mvwprintw(*edit_window, i, k, "%c", current_line->buf_[k]);
        //     wattroff(*edit_window, COLOR_PAIR(6));
        //
        //     line_len += end;
        //     rc2 = pcre2_match(re2, subject, subject_length, line_len, 0,
        //                       match_data2, NULL);
        // }

        // pcre2_match_data_free(match_data);

        current_line = current_line->next;
    }

    pcre2_code_free(re);
    update_line_numbers(tbuf, line_num_win);
}
