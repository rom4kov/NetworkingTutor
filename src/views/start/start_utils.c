#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <ncurses.h>

char *INTRODUCTION =
    "GET STARTED@\n\n"
    "Step 1: Customize Your Profile\n"
    "Navigate to the Account section in the navigation bar to change your name "
    "and personalize your experience.\n\n"
    "Step 2: Choose Your First Course\n"
    "Focus on one of the course cards (like \"Build an HTTP Server in C\") and "
    "press Enter to begin your learning journey.\n\n\n"
    "HOW THE COURSES WORK@\n\n"
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
    "points to unlock the next level and continue your learning adventure!\n\n\n" 
    "READY TO START?@\n\n"
    "Once you're ready, choose a course and dive in! The app is here to help "
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
