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

    print_progress_stats(ctx);

    wnoutrefresh(ctx->progress_windows[0]);
    wnoutrefresh(ctx->progress_windows[1]);
    doupdate();
}

WINDOW *create_progress_screen(APP_CONTEXT *ctx)
{
    WINDOW *progress_screen = newwin(LINES, COLS, 0, 0);

    // char date1[] = "2025-08-10 21:49:05";
    // char date2[] = "2025-08-13 21:49:05";
    // mvwprintw(progress_screen, 28, 10, "Test diff time: %d",
    //           get_diff_time_in_days(progress_screen, date1, date2));
    draw_progress_border(progress_screen, 2, "");

    return progress_screen;
}

void print_progress_stats(APP_CONTEXT *ctx)
{
    int current_streak = 0;
    wattron(ctx->progress_windows[1], A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->progress_windows[1], 8, 10, "%s", "PROGRESS");
    wattroff(ctx->progress_windows[1], A_UNDERLINE | A_BOLD);

    mvwprintw(ctx->progress_windows[1], 10, 10, "Name: %s", ctx->user_data->name);
    mvwprintw(ctx->progress_windows[1], 12, 10, "Account created: %s",
              ctx->user_data->created_at);
    mvwprintw(ctx->progress_windows[1], 14, 10, "Courses completed: %i",
              get_completed_courses(ctx));
    mvwprintw(ctx->progress_windows[1], 16, 10, "Sections completed: %i",
              get_total_completed_sections(ctx));
    mvwprintw(ctx->progress_windows[1], 18, 10, "Items completed: %i",
              get_total_completed_items(ctx));
    mvwprintw(ctx->progress_windows[1], 22, 10, "%s", "Longest streak:");
    mvwprintw(ctx->progress_windows[1], 24, 10, "%s", "Fastest section completion:");
    mvwprintw(ctx->progress_windows[1], 26, 10, "%s", "Slowest section completion:");
    mvwprintw(ctx->progress_windows[1], 20, 10, "%s",
              "Current streak: ");
    current_streak = get_current_streak(ctx);
    for (int i = 0; i < current_streak; i++) {
        mvwprintw(ctx->progress_windows[1], 20, 26 + i * 2, "%s",
                  "");
    }
}
