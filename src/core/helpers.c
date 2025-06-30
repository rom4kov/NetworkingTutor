#define _POSIX_C_SOURCE 200809L

#include "../models/models.h"
#include <math.h>
#include <curses.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <string.h>

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

void focus_window(WINDOW **window, int color_pair, char *label)
{
    draw_border(*window, color_pair, label);
    wattron(*window, COLOR_PAIR(3));
    mvwprintw(*window, 0, 2, " %s ", label);
    wattroff(*window, COLOR_PAIR(3));
    wnoutrefresh(*window);
}

void focus_instructions_window(RIGHT_PANEL_STATE *rps, int color_pair, char *label)
{
    draw_border(rps->right_panel, color_pair, label);
    wattron(rps->right_panel, COLOR_PAIR(3));
    mvwprintw(rps->right_panel, 0, 2, " %s ", label);
    mvwprintw(rps->right_panel, LINES - 4, 79, " %s %i of %i ",
              "Section", rps->curr_section + 1, 9);
    wattroff(rps->right_panel, COLOR_PAIR(3));

    wnoutrefresh(rps->right_panel);
}

void trim(char **str)
{
    char *end;

    while(isspace(*str[0])) *str += 1;

    end = *str + strlen(*str) - 1;
    while (end > *str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
}

char *return_trimmed(char *str)
{
    char *end;
    char *strcop = str;

    while(isspace(strcop[0])) strcop += 1;

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
    if (fract <= 0.1) return (int)floor(x);
    else return (int)ceil(x);
}
