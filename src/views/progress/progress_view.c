#include "../../core/core.h"
#include "../../data/data_access_layer.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_progress_view(APP_CONTEXT *ctx)
{
    ctx->progress_windows[3] = create_progress_screen(ctx);
    ctx->progress_windows[0] =
        create_navigation_window(&ctx->active_window_idx, &ctx->start_menu);
    ctx->progress_windows[1] = create_account_window(ctx);
    ctx->progress_windows[2] = create_progress_stats_window(ctx);

    create_user_form(ctx->progress_windows[1], &ctx->user_form,
                     &ctx->user_form_fields);

    wnoutrefresh(ctx->progress_windows[0]);
    wnoutrefresh(ctx->progress_windows[2]);
    wnoutrefresh(ctx->progress_windows[1]);
    wnoutrefresh(ctx->progress_windows[3]);
    wnoutrefresh(ctx->progress_windows[4]);
    doupdate();
    print_completed_courses(ctx);
}

WINDOW *create_progress_screen(APP_CONTEXT *ctx)
{
    WINDOW *progress_screen = newwin(LINES, COLS - 80, 0, 80);

    draw_progress_border(progress_screen, 2, 2);
    create_your_courses_window(ctx, progress_screen);

    return progress_screen;
}

WINDOW *create_account_window(APP_CONTEXT *ctx)
{
    WINDOW *account_window = newwin(LINES / 3 - 3, 79, 3, 0);
    WINDOW *account_title_win = derwin(account_window, 7, 40, 3, 8);
    wattron(account_title_win, A_BOLD);
    mvwprintw(account_title_win, 0, 0, "%s", get_ascii_art(ctx->db, "account"));
    wattroff(account_title_win, A_BOLD);
    draw_border(account_window, 2, NULL);

    wnoutrefresh(account_title_win);
    return account_window;
}

void create_your_courses_window(APP_CONTEXT *ctx, WINDOW *win)
{
    ctx->progress_windows[4] = derwin(win, 45, WU * 7 + 4, 6, 8);

    wattron(ctx->progress_windows[4], A_BOLD);
    mvwprintw(ctx->progress_windows[4], 0, 0, "%s",
              get_ascii_art(ctx->db, "your_courses"));
    wattroff(ctx->progress_windows[4], A_BOLD);
    // wrefresh(ctx->progress_windows[4]);
}

WINDOW *create_progress_stats_window(APP_CONTEXT *ctx)
{
    int current_streak = 0;
    int longest_streak = 0;
    int courses, sections, items;
    courses = sections = items = 0;

    WINDOW *stats_window = newwin(LINES - (LINES / 3), 79, (LINES / 3), 0);
    draw_border(stats_window, 2, NULL);

    WINDOW *inner_stats_win = derwin(stats_window, LINES / 3 * 2 - 6, 67, 3, 8);

    wattron(inner_stats_win, A_BOLD);
    mvwprintw(inner_stats_win, 0, 0, "%s", get_ascii_art(ctx->db, "progress"));
    wattroff(inner_stats_win, A_BOLD);

    mvwprintw(inner_stats_win, 6, 0, "Name: %s", ctx->user_data->name);
    mvwprintw(inner_stats_win, 8, 0, "Account created: %s",
              ctx->user_data->created_at);

    mvwprintw(inner_stats_win, 10, 0, "%s", "Current streak: ");
    current_streak = get_current_streak(ctx);
    for (int i = 0; i < current_streak; i++)
    {
        mvwprintw(inner_stats_win, 10, 16 + i * 2, "%s", "");
    }
    mvwprintw(inner_stats_win, 12, 0, "%s", "Longest streak:");
    longest_streak = get_longest_streak(ctx);
    for (int i = 0; i < longest_streak; i++)
    {
        mvwprintw(inner_stats_win, 12, 16 + i * 2, "%s", "");
    }

    courses = get_num_of_completed_courses(ctx);
    sections = get_total_completed_sections(ctx);
    items = get_total_completed_items(ctx);

    mvwprintw(inner_stats_win, 17, 0, "Courses completed: %i", courses);
    mvwprintw(inner_stats_win, 19, 0, "Sections completed: %i", sections);
    mvwprintw(inner_stats_win, 21, 0, "Items completed: %i", items);

    mvwprintw(inner_stats_win, 25, 0, "Experience Points: %i",
              get_num_of_completed_courses(ctx));
    mvwprintw(inner_stats_win, 27, 0, "Points until next rank: %i",
              get_total_completed_sections(ctx));
    mvwprintw(inner_stats_win, 29, 0, "Rank: %s", "Packet Novice");

    char *novice_ascii = "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡾⣿⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⢷⡃⠺⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢬⡋⠼⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢟⡫⢪⢳⠦⠂⢰⠈⠛⣿⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⣗⢾⢸⠇⡾⢠⢘⠀⠅⣸⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⢱⣏⢸⡰⠏⠔⠄⡇⡀⡂⢿⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⢸⣌⠐⣡⠟⠁⠀⠁⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣡⣶⠂⠓⠋⣀⠀⠀⢠⣧⢀⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣿⣿⣿⠸⠁⢿⡀⠀⠤⠉⣠⠋⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⡟⠄⠢⠤⠍⠐⠈⣝⣞⠇⢀⠤⠤⢀⡄⠙⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⠀⠀⠐⠤⠤⠤⠖⡡⠀⠀⠤⢐⠴⠃⠀⠀⣿⣿⣿⣿⣿⣿\n"
                         "⣿⣿⣿⣿⣿⣿⣷⣀⣀⣀⣀⣀⣁⣈⣁⣁⣀⣀⣀⣀⣀⣠⣛⣻⣿⣿⣿⣿";

    WINDOW *rank_window = derwin(stats_window, 13, 30, 20, 42);
    char *rank_ascii = "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠟⠿⣿⣿⣿⣿\n"
                       "⣿⣿⡟⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣤⡲⠆⢿⣿⣿\n"
                       "⣿⣿⡇⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠐⡖⣼⣿⣿\n"
                       "⣿⣿⣇⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠫⠀⠀⠹⣿⣿⡿⡃⠀⢠⣿⣿⣿\n"
                       "⣿⣿⣿⡆⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⡈⡤⠀⠋⣭⡔⠀⣠⣿⣿⣿⣿\n"
                       "⣿⣿⣿⣿⡄⠹⣿⣿⣿⣿⣿⣿⣿⠿⠟⠉⡀⠴⢇⣼⣿⠀⣰⣿⣿⣿⣿⣿\n"
                       "⣿⣿⣿⣿⣿⡄⠹⣿⣿⠿⡛⠉⠢⠞⠉⠀⠲⠶⠿⢿⠏⢸⣿⣿⣿⣿⣿⣿\n"
                       "⣿⣿⣿⣿⣿⣯⡀⠘⢇⣈⣁⣴⣶⣿⣿⣿⡄⠐⠾⡿⠁⣸⣿⣿⣿⣿⣿⣿\n"
                       "⣿⣿⣿⣿⣿⣿⣷⣤⣽⣿⣿⣿⣿⡿⠿⠛⠁⠀⠀⠀⠀⠉⣴⡾⢹⣿⣿⣿\n"
                       "⣿⣿⣿⣿⣿⣿⣿⠿⠿⠟⠛⠫⠵⠾⠟⠡⠀⠀⠀⠀⡀⠀⢀⡆⣸⣿⣿⣿\n"
                       "⣿⣿⣿⠟⠛⠵⠶⠿⠿⠂⢀⣀⣀⣀⢤⢃⣸⣿⣷⣿⠁⣠⡾⢱⣿⣿⣿⣿\n"
                       "⣿⣿⢫⠴⣀⣤⣀⣤⣶⣾⣿⣿⣿⣿⣷⣾⣿⣿⣿⡧⣀⣡⡠⣾⣿⣿⣿⣿\n"
                       "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣶⣿⣶⣿⣿⣿⣿⣿⣿";
    mvwprintw(rank_window, 0, 0, "%s", novice_ascii);
    wnoutrefresh(rank_window);

    wnoutrefresh(inner_stats_win);
    return stats_window;
}

void print_completed_courses(APP_CONTEXT *ctx)
{
    int number = 0;
    COURSE *completed_courses = get_completed_courses(ctx, &number);

    if (number > 0)
    {
        for (int i = 0; i < number; i++)
        {
            create_course_preview_card(ctx, 10 + (i * 46), 2 + i,
                                       &completed_courses[i]);
        }
    }
    else
    {
        char *msg = "You have not started any courses yet.";
        mvwprintw(ctx->progress_windows[3], LINES / 2,
                  ((COLS - 80) - strlen(msg)) / 2, "%s", msg);
        wrefresh(ctx->progress_windows[3]);
    }
}

void print_your_courses_title(APP_CONTEXT *ctx)
{
    wattron(ctx->progress_windows[3], A_BOLD);
    mvwprintw(ctx->progress_windows[3], 0, 0, "%s",
              get_ascii_art(ctx->db, "your_courses"));
    wattroff(ctx->progress_windows[3], A_BOLD);
}
