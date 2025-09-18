#include "../../../ntutor.h"
#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include "../../views/views.h"

#include <curses.h>
#include <ncurses.h>
#include <pcre2.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    PCRE2_UCHAR buffer[256];
    
    FILE *log_file = fopen("pcre2compile_log.txt", "a");

    for (int i = 0; i < p_codes_num; i++)
    {
        PCRE2_SPTR pattern = (PCRE2_SPTR)pattern_str[i];
        re[i] = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UCP,
                              &errcode, &erroffset, NULL);
	if (re[i] == NULL) {
		pcre2_get_error_message(errcode, buffer, sizeof(buffer));
		// fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n",
		//		(int)erroffset, buffer);
		char buf[512];
		memset(buf, 0, 512);
		buf[511] = '\0';
		snprintf(buf, 512, "PCRE2 compilation failed at offset %d: %s\n", (int)erroffset, buffer);
		fwrite(buf, 512, 1, log_file);
		fclose(log_file);
	}
    }
}

void print_matches(pcre2_code **re, int line_num, int j, size_t subject_length,
                   LINE *current_line, int group, WINDOW **edit_window,
                   int color)
{
    PCRE2_SPTR subject = (PCRE2_SPTR)current_line->buf_;
    if (re[j] == NULL) {
	    // fprintf(stderr, "ERROR: re[%d] is NULL in print_matches()\n", j);
	    return;
    }

    pcre2_match_data *md = pcre2_match_data_create_from_pattern(re[j], NULL);
    if (!md)
    {
        pcre2_match_data_free(md);
        return;
    }
    // FILE *log_file = fopen("print_matches_log.txt", "a");

    int rc = pcre2_match(re[j], subject, subject_length, 0, 0, md, NULL);
    if (j == 4 && rc > 0)
    {
        LINE *temp_line = NULL;
        char *extended_buf = calloc(500, sizeof(char));
        // LINE *orig_temp_line = temp_line;

        // char buf[40];
        // memset(buf, 0, 40);
        // buf[39] = '\0';
        // snprintf(buf, 40, "%p\n", temp_line);
        // fwrite(buf, 40, 1, log_file);

        temp_line = current_line;

        // memset(buf, 0, 40);
        // buf[39] = '\0';
        // snprintf(buf, 40, "%p\n", temp_line);
        // fwrite(buf, 40, 1, log_file);

        if (!temp_line)
        {
            pcre2_match_data_free(md);
            return;
        }

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

                // char buf[40];
                // memset(buf, 0, 40);
                // buf[39] = '\0';
                // snprintf(buf, 40, "%p - line: %s\n", temp_line,
                // temp_line->buf_); fwrite(buf, 40, 1, log_file);

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
        {
            free(extended_buf);
            pcre2_match_data_free(md);
            // free(orig_temp_line->buf_);
            // free(orig_temp_line);
            return;
        }

        extended_buf[strlen(extended_buf)] = '\0';

        subject_length = strlen(extended_buf);

        subject = (PCRE2_SPTR)extended_buf;
        pcre2_match_data_free(md);

        md = pcre2_match_data_create_from_pattern(re[3], NULL);

        if (!md)
        {
            free(extended_buf);
            // free(orig_temp_line->buf_);
            // free(orig_temp_line);
            pcre2_match_data_free(md);
            return;
        }

        rc = pcre2_match(re[3], subject, subject_length, 0, 0, md, NULL);

        free(extended_buf);
        // free(orig_temp_line->buf_);
        // free(orig_temp_line);
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
    // free(orig_temp_line->buf_);
    // free(orig_temp_line);
    // fclose(log_file);
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
        "\\b(void|int|ssize_t|size_t|socklen_t|char|return|for|while|if|else|"
        "break|continue|bool|switch|struct|case|default)\\b",
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
        "\\b(void|int|ssize_t|size_t|socklen_t|char|return|for|while|if|else|"
        "break|continue|bool|switch|struct|case|default)\\b",
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
    {
        pcre2_match_data_free(md);
        return NULL;
    }

    int rc = pcre2_match(re, subject, subj_len, 0, 0, md, NULL);

    if (rc > 0)
    {
        pcre2_match_data_free(md);
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

            for (int i = 0; i < pattern_num; i++)
                pcre2_code_free(re[i]);
            return matched_icon;
        }
    }

    for (int i = 0; i < pattern_num; i++)
        pcre2_code_free(re[i]);

    return matched_icon;
}

void print_buffer_label(APP_CONTEXT *ctx)
{

    mvwprintw(ctx->course_windows[2], 1, 4, "                                ");
    ICON icon = get_file_icon((char *)ctx->filename);

    wattron(ctx->course_windows[2], COLOR_PAIR(icon.color));
    mvwprintw(ctx->course_windows[2], 1, 5, "%s", icon.icon);
    wattroff(ctx->course_windows[2], COLOR_PAIR(icon.color));

    wattron(ctx->course_windows[2], A_BOLD | COLOR_PAIR(1));
    mvwprintw(ctx->course_windows[2], 1, 7, "%s", ctx->filename);
    wattroff(ctx->course_windows[2], A_BOLD | COLOR_PAIR(1));
}

void print_modified_marker(WINDOW *editor_window, int filename_len,
                           bool *file_modified)
{
    wattron(editor_window, A_BOLD | COLOR_PAIR(4));
    mvwprintw(editor_window, 1, 8 + filename_len, "");
    wattroff(editor_window, A_BOLD | COLOR_PAIR(4));
    *file_modified = true;
}

void print_file_metadata(APP_CONTEXT *ctx)
{
    fseek(ctx->file, 0, SEEK_END);
    ctx->file_size = ftell(ctx->file);
    rewind(ctx->file);

    if (ctx->file_size < 1000)
        mvwprintw(ctx->course_windows[2], ctx->editor_height - 2, 2, "%iB",
                  ctx->file_size);
    else
        mvwprintw(ctx->course_windows[2], ctx->editor_height - 2, 2, "%.1fk",
                  (1.0 * ctx->file_size / 1000));

    mvwprintw(ctx->course_windows[2], ctx->editor_height - 2, EDITOR_WIDTH - 7,
              "     ");
    print_cursor_position(&ctx->course_windows[2], ctx->t_buffer,
                          ctx->editor_height);
}
