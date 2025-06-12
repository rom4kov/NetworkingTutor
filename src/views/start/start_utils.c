#include "../../core/core.h"
#include <curses.h>
#include <ncurses.h>

char *INTRODUCTION =
    "Step 1: Customize Your Profile\n"
    "Navigate to the Account section in the navigation bar to change your name "
    "and personalize your experience.\n\n"
    "Step 2: Choose Your First Course\n"
    "Focus on one of the course cards (like \"Build an HTTP Server in C\") and "
    "press Enter to begin your learning journey.\n\n";

char *INTRODUCTION2 =
    "Step-by-Step Learning\n\n"
    "Each course is designed to guide you through small, manageable steps.\n"
    "You'll learn by:\n\n"
    "Short instructions to get you started.\n\n"
    "Reading material and links to deepen your understanding.\n\n"
    "Hands-on tasks to practice and apply what you've learned in the built-in "
    "editor.\n\n"
    "Testing and Feedback\n\n"
    "Once you've completed a task, you'll be prompted to test your solution. "
    "Submit it for evaluation to receive feedback and gain points. Use these "
    "points to unlock the next level and continue your learning adventure!\n\n";

char *INTRODUCTION3 =
    "Once you're ready, choose a course and dive in! The app is here to help "
    "guide you, and you'll learn while having fun.\n";

void print_intro(WINDOW **right_panel, int window_width, int intro_width)
{
    WINDOW *intro_win = derwin(*right_panel, LINES - 8, window_width - 6, 6, 3);
    wattron(intro_win, A_BOLD | A_UNDERLINE);
    mvwprintw(intro_win, 1, 0, "GET STARTED\n");
    wattroff(intro_win, A_BOLD | A_UNDERLINE);
    wprintw(intro_win, "\n");
    wprintw(intro_win, "%s", wrap_text(INTRODUCTION, intro_width, NULL));
    wprintw(intro_win, "\n");

    wattron(intro_win, A_BOLD | A_UNDERLINE);
    wprintw(intro_win, "HOW THE COURSES WORK\n");
    wattroff(intro_win, A_BOLD | A_UNDERLINE);
    wprintw(intro_win, "\n");
    wprintw(intro_win, "%s", wrap_text(INTRODUCTION2, intro_width, NULL));
    wprintw(intro_win, "\n");

    wattron(intro_win, A_BOLD | A_UNDERLINE);
    wprintw(intro_win, "READY TO START?\n");
    wattroff(intro_win, A_BOLD | A_UNDERLINE);
    wprintw(intro_win, "\n");
    wprintw(intro_win, "%s", wrap_text(INTRODUCTION3, intro_width, NULL));

    wnoutrefresh(intro_win);
}
