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
    ctx->progress_windows[2] = create_course_preview_card(
        10, &ctx->active_window, 0, &ctx->courses[0], ctx->course_view_active,
        ctx->progress_view_active);

    print_progress_stats(ctx);

    wnoutrefresh(ctx->progress_windows[0]);
    wnoutrefresh(ctx->progress_windows[1]);
    wnoutrefresh(ctx->progress_windows[2]);
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
    int longest_streak = 0;

    WINDOW *stats_window = derwin(ctx->progress_windows[1], 20, 50, 6, 10);

    wattron(stats_window, A_BOLD);
    mvwprintw(stats_window, 0, 0, "%s", get_ascii_art(ctx->db, "progress"));
    wattroff(stats_window, A_BOLD);

    mvwprintw(stats_window, 6, 0, "Name: %s", ctx->user_data->name);
    mvwprintw(stats_window, 8, 0, "Account created: %s",
              ctx->user_data->created_at);
    mvwprintw(stats_window, 10, 0, "Courses completed: %i",
              get_completed_courses(ctx));
    mvwprintw(stats_window, 12, 0, "Sections completed: %i",
              get_total_completed_sections(ctx));
    mvwprintw(stats_window, 14, 0, "Items completed: %i",
              get_total_completed_items(ctx));
    mvwprintw(stats_window, 16, 0, "%s", "Current streak: ");
    current_streak = get_current_streak(ctx);
    for (int i = 0; i < current_streak; i++)
    {
        mvwprintw(stats_window, 16, 16 + i * 2, "%s", "");
    }
    mvwprintw(stats_window, 18, 0, "%s", "Longest streak:");
    longest_streak = get_longest_streak(ctx);
    for (int i = 0; i < longest_streak; i++)
    {
        mvwprintw(stats_window, 18, 16 + i * 2, "%s", "");
    }

    wrefresh(stats_window);
}

void print_completed_courses(APP_CONTEXT *ctx)
{
}
