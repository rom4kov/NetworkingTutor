#include "../../../ntutor.h"
#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_all_courses_view(APP_CONTEXT *ctx)
{
    ctx->all_courses_windows[1] = create_all_courses_screen();
    ctx->all_courses_windows[0] = create_navigation_window(ctx);
    ctx->all_courses_windows[2] = create_all_courses_title_window(ctx);

    wnoutrefresh(ctx->all_courses_windows[0]);
    wnoutrefresh(ctx->all_courses_windows[1]);
    wnoutrefresh(ctx->all_courses_windows[2]);
    doupdate();
    print_all_courses(ctx);
}

WINDOW *create_all_courses_screen()
{
    WINDOW *all_courses_screen = newwin(LINES, COLS, 0, 0);
    draw_progress_border(all_courses_screen, 2, 1);
    return all_courses_screen;
}

WINDOW *create_all_courses_title_window(APP_CONTEXT *ctx)
{
    WINDOW *all_courses_title_window =
        derwin(ctx->all_courses_windows[1], 4, 44, 2, WU * 8 + 10);

    char *all_courses_ascii = get_ascii_art(ctx->db, "all_courses");

    wattron(all_courses_title_window, A_BOLD);
    mvwprintw(all_courses_title_window, 0, 0, "%s", all_courses_ascii);
    wattroff(all_courses_title_window, A_BOLD);

    free(all_courses_ascii);

    return all_courses_title_window;
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
            create_course_preview_card(ctx, 4 + x_pos, 2 + i, &ctx->courses[i]);
            if ((x_pos + 30) <= COLS)
            {
                x_pos += (COLS - 16) / 5 + 2;
            }
        }
    }
    else
    {
        char *msg = "No courses in the database.";
        mvwprintw(ctx->all_courses_windows[2], LINES / 2,
                  ((COLS - 80) - strlen(msg)) / 2, "%s", msg);
        wrefresh(ctx->all_courses_windows[2]);
    }
}
