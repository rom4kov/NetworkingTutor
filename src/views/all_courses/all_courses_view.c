#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_all_courses_view(APP_CONTEXT *ctx)
{
    ctx->all_courses_windows[1] = create_all_courses_screen(ctx);
    ctx->all_courses_windows[0] =
        create_navigation_window(&ctx->active_window_idx, &ctx->start_menu);
    ctx->all_courses_windows[2] = create_all_courses_container(ctx);

    wnoutrefresh(ctx->all_courses_windows[0]);
    wnoutrefresh(ctx->all_courses_windows[1]);
    wnoutrefresh(ctx->all_courses_windows[2]);
    doupdate();
    print_all_courses(ctx);
}

WINDOW *create_all_courses_screen(APP_CONTEXT *ctx)
{
    WINDOW *all_courses_screen = newwin(LINES, COLS, 0, 0);
    draw_progress_border(all_courses_screen, 2, 1);
    return all_courses_screen;
}

WINDOW *create_all_courses_container(APP_CONTEXT *ctx)
{
    WINDOW *all_courses_container =
        derwin(ctx->all_courses_windows[1], LINES - 10, COLS - 16, 6, 8);

    wattron(all_courses_container, A_BOLD);
    mvwprintw(all_courses_container, 0, 0, "%s",
              get_ascii_art(ctx->db, "all_courses"));
    wattroff(all_courses_container, A_BOLD);

    return all_courses_container;
}

void print_all_courses(APP_CONTEXT *ctx)
{
    int number = 10;
    int x_pos = 0;

    if (number > 0)
    {
        for (int i = 0; i < number; i++)
        {
            if ((x_pos + 30) > COLS)
            {
                x_pos = 0;
            }
            create_course_preview_card(ctx, 8 + x_pos, 2 + i, &ctx->courses[i]);
            if ((x_pos + 30) <= COLS)
            {
                x_pos += (COLS - 16) / 5;
            }
        }
    }
    else
    {
        char *msg = "No courses in the database.";
        mvwprintw(ctx->progress_windows[3], LINES / 2,
                  ((COLS - 80) - strlen(msg)) / 2, "%s", msg);
        wrefresh(ctx->progress_windows[3]);
    }
}
