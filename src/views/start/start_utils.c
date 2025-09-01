#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <ncurses.h>

char *INTRODUCTION =
    "GET STARTED@\n\n"
    "Start with the First Course\n\n"
    "Your adventure begins at the very first course (e.g., Build an HTTP Server "
    "in C). Select it and press Enter to dive in.\n\n"
    "Later courses will unlock as you progress. Those still unavailable are clearly "
    "marked as Locked in the course selection view.\n\n\n\n"
    "HOW THE COURSES WORK@\n\n"
    "A Guided Path\n\n"
    "The courses follow a structured trajectory. Many build on knowledge from"
    " earlier lessons, though not always in a strictly linear way. Think of"
    " them as stepping stones that gradually expand your skills.\n\n\n"
    "You'll learn through:\n\n"
    "• Short, focused instructions to get you started.\n\n"
    "• Reading material and links to deepen your understanding.\n\n"
    "• Hands-on coding tasks to practice and apply what you've learned in the built-in "
    "editor.\n\n\n"
    "Practice, Test, Unlock\n\n"
    "After each task, you’ll test your solution. Submit it to get instant feedback "
    "and earn points. Completing tasks and courses unlocks the next stages of your "
    "path, helping you steadily progress.\n\n\n\n"
    "READY TO START?@\n\n"
    "Once you're ready, start with the first course and dive in! The app is here to help "
    "guide you, and you'll learn while having fun.\n";

void print_intro(APP_CONTEXT *ctx)
{
    ctx->intro_buffer = initialize_it_buffer();

    read_window_text_into_buffer(ctx, ctx->intro_buffer, ctx->rp_state->window_width, 2, 0, INTRODUCTION);

    wattron(ctx->rp_state->right_panel, A_BOLD | A_UNDERLINE);
    mvwprintw(ctx->rp_state->right_panel, 2, (ctx->rp_state->window_width - 12) / 2, "%s", "INTRODUCTION");
    wattroff(ctx->rp_state->right_panel, A_BOLD | A_UNDERLINE);

    print_window_content(ctx->intro_buffer, ctx->rp_state->inner_win, ctx->rp_state->window_width - 4);
}
