#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE 1

#include "../models/models.h"
#include "../views/views.h"
#include "fcntl.h"
#include "unistd.h"
#include <ctype.h>
#include <curses.h>
#include <math.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void draw_border(WINDOW *win, int color_pair, char *label)
{
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    wattron(win, COLOR_PAIR(color_pair));

    // Draw top and bottom borders
    for (int i = 1; i < max_x - 1; i++)
    {
        mvwaddch(win, 0, i, ACS_HLINE);
        mvwaddch(win, max_y - 1, i, ACS_HLINE);
    }

    // Draw left and right borders
    for (int i = 1; i < max_y - 1; i++)
    {
        mvwaddch(win, i, 0, ACS_VLINE);
        mvwaddch(win, i, max_x - 1, ACS_VLINE);
    }

    mvwprintw(win, 0, 0, "╭");
    mvwprintw(win, 0, max_x - 1, "╮");
    mvwprintw(win, max_y - 1, 0, "╰");
    mvwprintw(win, max_y - 1, max_x - 1, "╯");

    if (NULL != label && strcmp(label, "Lesson") == 0)
    {
        mvwaddch(win, 2, 0, ACS_LTEE);
        mvwaddch(win, 2, max_x - 1, ACS_RTEE);
    }

    wattroff(win, COLOR_PAIR(color_pair));
    wnoutrefresh(win);
}

void draw_progress_border(WINDOW *win, int color_pair, int screen)
{
    int angle_pos = 0;
    if (screen == 1)
    {
        angle_pos = 5;
    }
    else if (screen == 2) {
        angle_pos = -75;
    }
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    wattron(win, COLOR_PAIR(color_pair));

    // Draw top and bottom borders
    for (int i = 0; i < max_x - 1; i++)
    {
        mvwaddch(win, i < (WU * 7 + angle_pos) ? 3 : 0, i, ACS_HLINE);
        mvwaddch(win, max_y - 1, i, ACS_HLINE);
    }

    // Draw left and right borders
    for (int i = 1; i < max_y - 1; i++)
    {
        mvwaddch(win, i, i < 4 ? (WU * 7 + angle_pos) : 0, ACS_VLINE);
        mvwaddch(win, i, max_x - 1, ACS_VLINE);
    }

    mvwprintw(win, 3, 0, "╭");
    mvwprintw(win, 0, WU * 7 + angle_pos, "╭");
    mvwprintw(win, 0, max_x - 1, "╮");
    mvwprintw(win, max_y - 1, 0, "╰");
    mvwprintw(win, 3, WU * 7 + angle_pos, "╯");
    mvwprintw(win, max_y - 1, max_x - 1, "╯");

    wattroff(win, COLOR_PAIR(color_pair));
    wnoutrefresh(win);
}

void focus_window(WINDOW **window, int color_pair, char *label)
{
    draw_border(*window, color_pair, label);
    if (strcmp(label, "") != 0)
    {
        wattron(*window, COLOR_PAIR(3));
        mvwprintw(*window, 0, 2, " %s ", label);
        wattroff(*window, COLOR_PAIR(3));
    }
    wnoutrefresh(*window);
}

void focus_instructions_window(RIGHT_PANEL_STATE *rps, int color_pair,
                               char *label)
{
    draw_border(rps->right_panel, color_pair, label);
    wattron(rps->right_panel, COLOR_PAIR(3));
    mvwprintw(rps->right_panel, 0, 2, " %s ", label);
    if (!rps->showing_end_of_course_page)
    {
        mvwprintw(rps->right_panel, LINES - 4, rps->window_width - 18,
                  " %s %i of %i ", "Section", rps->curr_section + 1,
                  rps->total_course_sections);
    }
    wattroff(rps->right_panel, COLOR_PAIR(3));

    wnoutrefresh(rps->right_panel);
}
//
// void focus_progress_window(APP_CONTEXT *ctx, int color_pair)
// {
//     draw_progress_border(ctx->progress_windows[3], color_pair, "");
//     print_completed_courses(ctx);
// }

void trim(char **str)
{
    char *end;

    while (isspace(*str[0]))
        *str += 1;

    end = *str + strlen(*str) - 1;
    while (end > *str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
}

char *return_trimmed(char *str)
{
    char *end;
    char *strcop = str;

    while (isspace(strcop[0]))
        strcop += 1;

    end = strcop + strlen(strcop) - 1;
    while (end > strcop && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';

    return strcop;
}

char *wrap_text(char *text, int width)
{
    char *str = strdup(text);
    int i, col, last_space, offset;
    i = col = last_space = offset = 0;
    while (str[i])
    {
        if (str[i] == '\n')
        {
            col = 0;
        }
        if (col > width - 1)
        {
            str[last_space] = '\n';
            col = 0;
            // offset++;
        }
        else if (str[i] == ' ')
        {
            last_space = i;
        }
        col++;
        i++;
    }

    // if (curr_offset != NULL)
    // {
    //     *curr_offset = *curr_offset + offset + 3;
    // }

    return str;
}

char *trunc_str(char *str, int win_width, int offset)
{
    char *str_copy = malloc(30 * sizeof(char));

    strcpy(str_copy, str);
    str_copy[win_width - offset] = '\0';

    return str_copy;
}

int c_round(float x)
{
    float fract = x - floor(x);
    if (fract <= 0.1)
        return (int)floor(x);
    else
        return (int)ceil(x);
}

char *current_datetime()
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char *datetime = (char *)malloc(32);
    if (!datetime)
        return NULL;
    size_t written = strftime(datetime, 32, "%Y-%m-%d %H:%M:%S", tm_info);
    if (written == 0)
    {
        free(datetime);
        return NULL;
    }
    return datetime;
}

int get_diff_time_in_days(WINDOW *win, char date1[], char date2[])
{
    char *time1 = malloc(20);
    char *time2 = malloc(20);
    strcpy(time1, date1);
    strcpy(time2, date2);

    double time_diff_in_seconds = 0;
    int diff_in_days = 0;
    struct tm start_date;
    struct tm end_date;
    time_t start_time, end_time;

    char delim[] = "-";

    start_date.tm_year = atoi(strsep(&time1, delim)) - 1900;
    start_date.tm_mon = atoi(strsep(&time1, delim)) - 1;
    start_date.tm_mday = atoi(strsep(&time1, delim));
    start_date.tm_hour = 0;
    start_date.tm_min = 0;
    start_date.tm_sec = 0;

    end_date.tm_year = atoi(strsep(&time2, delim)) - 1900;
    end_date.tm_mon = atoi(strsep(&time2, delim)) - 1;
    end_date.tm_mday = atoi(strsep(&time2, delim));
    end_date.tm_hour = 0;
    end_date.tm_min = 0;
    end_date.tm_sec = 0;

    // mvwprintw(win, 40, 10, "%i", start_date.tm_year);
    // mvwprintw(win, 41, 10, "%i", start_date.tm_mon);
    // mvwprintw(win, 42, 10, "%i", start_date.tm_mday);
    //
    // mvwprintw(win, 44, 10, "%i", end_date.tm_year);
    // mvwprintw(win, 45, 10, "%i", end_date.tm_mon);
    // mvwprintw(win, 46, 10, "%i", end_date.tm_mday);

    start_time = mktime(&start_date);
    end_time = mktime(&end_date);

    // mvwprintw(win, 40, 20, "start_time: %ld", start_time / (3600 * 24));
    // mvwprintw(win, 41, 20, "end_time: %ld", end_time / (3600 * 24));

    time_diff_in_seconds = difftime(end_time, start_time);

    // mvwprintw(win, 42, 20, "time diff in days: %.f",
    //           time_diff_in_seconds / (3600 * 24));

    diff_in_days = (int)time_diff_in_seconds / (3600 * 24);
    free(time1);
    free(time2);
    return diff_in_days;
}

int suppress_stdout(void)
{
    int dev_null = open("/dev/null", O_WRONLY);
    if (dev_null == -1)
        return -1;
    return dup2(dev_null, STDOUT_FILENO);
}

int restore_stdout(int saved_stdout)
{
    return dup2(saved_stdout, STDOUT_FILENO);
}
