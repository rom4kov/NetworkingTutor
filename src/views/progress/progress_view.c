#include "../../core/core.h"
#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>

void create_progress_view(APP_CONTEXT *ctx)
{
    ctx->progress_windows[3] = create_progress_screen(ctx);
    ctx->progress_windows[0] =
        create_navigation_window(&ctx->active_window, &ctx->start_menu);
    ctx->progress_windows[1] = create_account_window(ctx);
    ctx->progress_windows[2] = create_progress_stats_window(ctx);
    // ctx->progress_windows[3] = create_course_preview_card(
    //     10, &ctx->active_window, 0, &ctx->courses[0],
    //     ctx->course_view_active, ctx->progress_view_active);

    create_user_form(ctx->progress_windows[1], &ctx->user_form, &ctx->user_form_fields);

    wnoutrefresh(ctx->progress_windows[0]);
    wnoutrefresh(ctx->progress_windows[2]);
    wnoutrefresh(ctx->progress_windows[1]);
    wnoutrefresh(ctx->progress_windows[3]);
    doupdate();
}

WINDOW *create_progress_screen(APP_CONTEXT *ctx)
{
    WINDOW *progress_screen = newwin(LINES, COLS, 0, 0);

    wattron(progress_screen, A_BOLD);
    mvwprintw(progress_screen, 0, 0, "%s",
              get_ascii_art(ctx->db, "your_courses"));
    wattroff(progress_screen, A_BOLD);

    draw_progress_border(progress_screen, 2, "");
    create_your_courses_window(ctx);
    print_completed_courses(ctx);

    return progress_screen;
}

WINDOW *create_account_window(APP_CONTEXT *ctx)
{
    WINDOW *account_window = newwin(LINES / 2 - 3, 79, 3, 0);
    WINDOW *account_title_win = derwin(account_window, 7, 40, 3, 8);
    wattron(account_title_win, A_BOLD);
    mvwprintw(account_title_win, 0, 0, "%s", get_ascii_art(ctx->db, "account"));
    wattroff(account_title_win, A_BOLD);
    draw_border(account_window, 2, NULL);

    wnoutrefresh(account_title_win);
    return account_window;
}

WINDOW *create_your_courses_window(APP_CONTEXT *ctx)
{
    WINDOW *your_courses_window =
        newwin(LINES - 9, (COLS / 5) * 3 + 3, 6, (COLS / 5) * 2 - 3);

    wattron(your_courses_window, A_BOLD);
    mvwprintw(your_courses_window, 0, 0, "%s",
              get_ascii_art(ctx->db, "your_courses"));
    wattroff(your_courses_window, A_BOLD);

    // draw_border(your_courses_window, 2, "");
    wnoutrefresh(your_courses_window);

    return your_courses_window;
}

WINDOW *create_progress_stats_window(APP_CONTEXT *ctx)
{
    int current_streak = 0;
    int longest_streak = 0;

    WINDOW *stats_window = newwin(LINES / 2, 79, LINES / 2, 0);
    draw_border(stats_window, 2, NULL);

    WINDOW *inner_stats_win = derwin(stats_window, LINES / 2 - 6, 67, 3, 8);

    wattron(inner_stats_win, A_BOLD);
    mvwprintw(inner_stats_win, 0, 0, "%s", get_ascii_art(ctx->db, "progress"));
    wattroff(inner_stats_win, A_BOLD);

    mvwprintw(inner_stats_win, 6, 0, "Name: %s", ctx->user_data->name);
    mvwprintw(inner_stats_win, 8, 0, "Account created: %s",
              ctx->user_data->created_at);
    mvwprintw(inner_stats_win, 10, 0, "Courses completed: %i",
              get_num_of_completed_courses(ctx));
    mvwprintw(inner_stats_win, 12, 0, "Sections completed: %i",
              get_total_completed_sections(ctx));
    mvwprintw(inner_stats_win, 14, 0, "Items completed: %i",
              get_total_completed_items(ctx));
    mvwprintw(inner_stats_win, 16, 0, "%s", "Current streak: ");
    current_streak = get_current_streak(ctx);
    for (int i = 0; i < current_streak; i++)
    {
        mvwprintw(inner_stats_win, 16, 16 + i * 2, "%s", "");
    }
    mvwprintw(inner_stats_win, 18, 0, "%s", "Longest streak:");
    longest_streak = get_longest_streak(ctx);
    for (int i = 0; i < longest_streak; i++)
    {
        mvwprintw(inner_stats_win, 18, 16 + i * 2, "%s", "");
    }

    wnoutrefresh(inner_stats_win);
    return stats_window;
}

void print_completed_courses(APP_CONTEXT *ctx)
{
    int number = 0;
    COURSE *completed_courses = get_completed_courses(ctx, &number);

    for (int i = 0; i < number; i++)
    {
        create_course_preview_card(ctx, 10 + (i * 46), 2 + i,
                                   &completed_courses[i]);
    }
}

void print_your_courses_title(APP_CONTEXT *ctx)
{
    wattron(ctx->progress_windows[3], A_BOLD);
    mvwprintw(ctx->progress_windows[3], 0, 0, "%s",
              get_ascii_art(ctx->db, "your_courses"));
    wattroff(ctx->progress_windows[3], A_BOLD);
}
