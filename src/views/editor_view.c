// #include <pcre.h>
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

void print_buffer(TEXT_BUFFER *tbuf, WINDOW **edit_window,
                  WINDOW **line_num_win)
{
    werase(*line_num_win);
    werase(*edit_window);
    // int word_len = 0;
    // int line_len = 0;

    // char *word;

    // int error;
    // regex_t reg;
    // regmatch_t pm;

    LINE *current_line = tbuf->first_line;

    pcre2_code *re;
    pcre2_match_data *match_data;
    int errcode;
    PCRE2_SIZE erroffset;

    // Define the pattern and subject
    PCRE2_SPTR pattern = (PCRE2_SPTR) "\\b(void|int|return)\\b";
    // PCRE2_SPTR subject = (PCRE2_SPTR)current_line->buf_;
    // size_t subject_length = strlen(current_line->buf_);

    // Compile the regex pattern
    re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UCP, &errcode, &erroffset,
                       NULL);
    if (!re)
        return; // Compilation failed

    // Create match data
    match_data = pcre2_match_data_create_from_pattern(re, NULL);

    int rc;
    int line_len;
    // PCRE2_SIZE *ovector;

    // rc = pcre2_match(re, subject, subject_length, 0, 0, match_data, NULL);

    for (int i = 0; i < tbuf->num_of_lines - 1; i++)
    {
        // for (int j = 0; j < current_line->length; j++)
        // {
        PCRE2_SPTR subject = (PCRE2_SPTR)current_line->buf_;
        if (subject == NULL)
        { // Check for NULL buffer
            current_line = current_line->next;
            continue;
        }

        size_t subject_length = strlen(current_line->buf_);

        // Create match data (per line)
        match_data = pcre2_match_data_create_from_pattern(re, NULL);
        if (!match_data)
        {
            pcre2_code_free(re);
            return;
        }

        line_len = 0;
        rc = pcre2_match(re, subject, subject_length, 0, 0, match_data, NULL);

        while (rc > 0)
        {
            PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
            PCRE2_SIZE start = ovector[0];
            PCRE2_SIZE end = ovector[1];
            //
            for (PCRE2_SIZE k = start; k < end; k++)
                mvwprintw(*edit_window, i, k, "%c", current_line->buf_[k]);
            //
            line_len += end;
            //
            rc = pcre2_match(re, subject, subject_length, line_len, 0,
                             match_data, NULL);
            // rc = 0;
        }

        pcre2_match_data_free(match_data);

        // if (regcomp(&reg, "[a-z]{3}", REG_EXTENDED) != 0) return;
        // error = regexec(&reg, current_line->buf_, 1, &pm, 0);
        // while (error == 0)
        // {
        //     for (int k = line_len + pm.rm_so; k < line_len + pm.rm_eo; k++)
        //         mvwprintw(*edit_window, i, k, "%c", current_line->buf_[k]);
        //
        //     line_len += pm.rm_eo;
        //     error = regexec(&reg, line_len + current_line->buf_, 1, &pm,
        //     REG_NOTBOL);
        // }
        // regfree(&reg);

        //     if (current_line->buf_[j] == ' ' || current_line->buf_[j] == '\n'
        //     ||
        //         current_line->buf_[j] == '(' || current_line->buf_[j] == ')')
        //     {
        //         word_len++;
        //         mvwprintw(*edit_window, LINES + i - 20, line_len + 1, "%i",
        //         word_len); word = malloc(sizeof(word_len)); strncpy(word,
        //         &current_line->buf_[line_len], word_len); word[word_len - 1]
        //         = '\0';
        //
        //         // mvwprintw(*edit_window, LINES + i - 25, line_len, "%s",
        //         // word);
        //         if (strcmp(word, "#include") == 0)
        //         {
        //             wattron(*edit_window, COLOR_PAIR(6));
        //         }
        //         if (strcmp(word, "<stdio.h>") == 0)
        //         {
        //             wattron(*edit_window, COLOR_PAIR(4));
        //         }
        //         if (strcmp(word, "int") == 0 || strcmp(word, "void") == 0 ||
        //             strcmp(word, "return") == 0)
        //         {
        //             wattron(*edit_window, COLOR_PAIR(8));
        //         }
        //
        //         for (int k = 0; k < word_len; k++)
        //         {
        //             mvwprintw(*edit_window, i, line_len + k, "%c",
        //                       current_line->buf_[line_len + k]);
        //
        //             if (k == word_len - 2)
        //             {
        //                 if (strcmp(word, "#include") == 0)
        //                 {
        //                     wattroff(*edit_window, COLOR_PAIR(6));
        //                 }
        //                 if (strcmp(word, "<stdio.h>") == 0)
        //                 {
        //                     wattroff(*edit_window, COLOR_PAIR(4));
        //                 }
        //                 if (strcmp(word, "int") == 0 || strcmp(word, "void")
        //                 == 0 ||
        //                     strcmp(word, "return") == 0)
        //                 {
        //                     wattroff(*edit_window, COLOR_PAIR(8));
        //                 }
        //             }
        //         }
        //
        //         line_len += word_len;
        //         word_len = 0;
        //         free(word);
        //     }
        //     else
        //     {
        //         word_len++;
        //         mvwprintw(*edit_window, LINES + i - 20, line_len + 1, "%i",
        //         word_len);
        //     }
        // }
        // line_len = 0;
        // word_len = 0;
        current_line = current_line->next;
    }
    pcre2_code_free(re);
    update_line_numbers(tbuf, line_num_win);
}
