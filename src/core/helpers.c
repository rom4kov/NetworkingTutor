#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <ncurses.h>
#include <string.h>
// #include <stdio.h>

// char *wrap_text2(char *text, int width);

// char *INTRODUCTION2 = 
//     "Step-by-Step Learning\n"
//     "Each course is designed to guide you through small, manageable steps.\n"
//     "You'll learn by:\n"
//     "Short instructions to get you started.\n"
//     "Reading material and links to deepen your understanding.\n"
//     "Hands-on tasks to practice and apply what you've learned in the built-in "
//     "editor.\n"
//     "Testing and Feedback\n"
//     "Once you've completed a task, you'll be prompted to test your solution. "
//     "Submit it for evaluation to receive feedback and gain points. Use these "
//     "points to unlock the next level and continue your learning adventure!\n\n";

// int main(void)
// {
//     char *text = wrap_text2(INTRODUCTION2, 58);
//     printf("%s", text);
// }

void draw_border(WINDOW *win, int color_pair, int active_window)
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
    if (color_pair == 2 && active_window > 2 && active_window < 4)
    {
        mvwaddch(win, 2, 0, ACS_LTEE);
        mvwaddch(win, 2, max_x - 1, ACS_RTEE);
    }

    wattroff(win, COLOR_PAIR(color_pair));
    wrefresh(win);
}

void focus_window(WINDOW **windows, int window, int color_pair,
                  int *active_window, char *label)
{
    draw_border(windows[window], color_pair, *active_window);
    wattron(windows[window], COLOR_PAIR(3));
    mvwprintw(windows[window], 0, 2, "%s", label);
    wattroff(windows[window], COLOR_PAIR(3));
    wrefresh(windows[window]);
}

void trim(char *str)
{
    char *end;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
}

int get_length(char *str)
{
    int i = 0;
    while (str[i] != ' ')
    {
        i++;
    }
    return i;
}

char *wrap_text(char *text, int width)
{
    char *str = strdup(text);
    int i, col, last_space;
    i = col = last_space = 0;
    while (str[i])
    {
        // printf("i: %i, col: %i\n", i, col);
        // printf("last_space: %i\n", last_space);
        // printf("text at %i: %c\n", i, text[i]);
        if (str[i] == '\n')
        {
            col = 0;
        }
        if (col > width)
        {
            str[last_space] = '\n';
            col = 0;
        }
        else if (str[i] == ' ')
        {
            last_space = i;
        }
        col++;
        i++;
    }

    return str;
}
