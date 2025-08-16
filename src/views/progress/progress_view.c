#include "../../core/core.h"
#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>

void create_progress_view(APP_CONTEXT *ctx)
{
    ctx->progress_windows[1] = create_progress_screen(ctx);
    ctx->progress_windows[0] =
        create_navigation_window(&ctx->active_window, &ctx->start_menu);

    wnoutrefresh(ctx->progress_windows[0]);
    wnoutrefresh(ctx->progress_windows[1]);
    doupdate();
}

WINDOW *create_progress_screen(APP_CONTEXT *ctx)
{
    WINDOW *progress_screen = newwin(LINES, COLS, 0, 0);

    wattron(progress_screen, A_UNDERLINE | A_BOLD);
    mvwprintw(progress_screen, 8, 10, "%s", "PROGRESS");
    wattroff(progress_screen, A_UNDERLINE | A_BOLD);

    mvwprintw(progress_screen, 10, 10, "Name: %s", ctx->user_data->name);
    mvwprintw(progress_screen, 12, 10, "Account created: %s",
              ctx->user_data->created_at);
    mvwprintw(progress_screen, 14, 10, "Courses completed: %i",
              get_completed_courses(ctx));
    mvwprintw(progress_screen, 16, 10, "Sections completed: %i",
              get_total_completed_sections(ctx));
    mvwprintw(progress_screen, 18, 10, "Items completed: %i",
              get_total_completed_items(ctx));
    mvwprintw(progress_screen, 20, 10, "%s", "Current streak:");
    mvwprintw(progress_screen, 22, 10, "%s", "Longest streak:");
    mvwprintw(progress_screen, 24, 10, "%s", "Fastest section completion:");
    mvwprintw(progress_screen, 26, 10, "%s", "Slowest section completion:");
    draw_progress_border(progress_screen, 2, "");
    wrefresh(ctx->progress_windows[1]);

    return progress_screen;
}
