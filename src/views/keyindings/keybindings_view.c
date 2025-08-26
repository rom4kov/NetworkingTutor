#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_keybindings_view(APP_CONTEXT *ctx)
{
    ctx->keybindings_windows[1] = create_keybindings_screen(ctx);
    ctx->keybindings_windows[0] =
        create_navigation_window(&ctx->active_window_idx, &ctx->start_menu);
    ctx->keybindings_windows[2] = create_keybindings_container(ctx);

    wnoutrefresh(ctx->keybindings_windows[0]);
    wnoutrefresh(ctx->keybindings_windows[1]);
    wnoutrefresh(ctx->keybindings_windows[2]);
    doupdate();
    // print_keybindings(ctx);
}

WINDOW *create_keybindings_screen(APP_CONTEXT *ctx)
{
    WINDOW *keybindings_screen = newwin(LINES, COLS, 0, 0);
    draw_progress_border(keybindings_screen, 2, 1);
    return keybindings_screen;
}

WINDOW *create_keybindings_container(APP_CONTEXT *ctx)
{
    WINDOW *keybindings_container =
        derwin(ctx->keybindings_windows[1], LINES - 10, COLS - 16, 6, 8);

    wattron(keybindings_container, A_BOLD);
    mvwprintw(keybindings_container, 0, 0, "%s",
              get_ascii_art(ctx->db, "keybindings"));
    wattroff(keybindings_container, A_BOLD);

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 7, 0, "%s",
              "All views");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 9, 0, "%s",
              "Move between windows: Arrow keys ← ↑ → ↓");
    mvwprintw(keybindings_container, 11, 0, "%s",
              "Activate a window: move to window and press ENTER");
    mvwprintw(keybindings_container, 13, 0, "%s",
              "Deactivate a window: F1");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 16, 0, "%s",
              "Navbar");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 18, 0, "%s",
              "Go to different screen: Left / Right keys ← → and press Enter");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 21, 0, "%s",
              "Start view");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 23, 0, "%s",
              "Go back to greeter screen: press Enter on logo window");
    mvwprintw(keybindings_container, 25, 0, "%s",
              "Go to a course: move to course card and press Enter");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 28, 0, "%s",
              "Editor window");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 30, 0, "%s",
              "Move cursor: Arrow keys ← ↑ → ↓");
    mvwprintw(keybindings_container, 32, 0, "%s",
              "Move to start of line: Home key");
    mvwprintw(keybindings_container, 34, 0, "%s",
              "Move to end of line: End key");
    mvwprintw(keybindings_container, 36, 0, "%s",
              "Save file: F10");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 7, 80, "%s",
              "Explorer window");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 9, 80, "%s",
              "Move up and down: up and down keys ↑ ↓");
    mvwprintw(keybindings_container, 11, 80, "%s",
              "Open file or directory: press Enter when on item");
    mvwprintw(keybindings_container, 13, 80, "%s",
              "Create new file: move to directory where you");
    mvwprintw(keybindings_container, 14, 98, "%s",
              "want to create the file and press 'a' key");
    mvwprintw(keybindings_container, 16, 80, "%s",
              "Create new directory: 'm' key");
    mvwprintw(keybindings_container, 18, 80, "%s",
              "Rename a file or directory: 'r' key");
    mvwprintw(keybindings_container, 20, 80, "%s",
              "Delete file or directory: 'd' key");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 23, 80, "%s",
              "Course instructions window");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 25, 80, "%s",
              "Scroll up and down: up and down keys ↑ ↓");
    mvwprintw(keybindings_container, 27, 80, "%s",
              "Move between completed course sections: '<' and '>' keys");
    mvwprintw(keybindings_container, 29, 80, "%s",
              "Show next paragraph / text item: space key");
    mvwprintw(keybindings_container, 31, 80, "%s",
              "Show separate task if there is one: 't' key");
    mvwprintw(keybindings_container, 33, 80, "%s",
              "Submit your the code you've written: 's' key");
    mvwprintw(keybindings_container, 35, 80, "%s",
              "Complete current section and go to next one: ENTER");

    return keybindings_container;
}

// void print_keybindings(APP_CONTEXT *ctx)
// {
//     int number = 10;
//     int x_pos = 0;
//
//     if (number > 0)
//     {
//         for (int i = 0; i < number; i++)
//         {
//             if ((x_pos + 30) > COLS)
//             {
//                 x_pos = 0;
//             }
//             create_course_preview_card(ctx, 8 + x_pos, 2 + i, &ctx->courses[i]);
//             if ((x_pos + 30) <= COLS)
//             {
//                 x_pos += (COLS - 16) / 5;
//             }
//         }
//     }
//     else
//     {
//         char *msg = "No courses in the database.";
//         mvwprintw(ctx->progress_windows[3], LINES / 2,
//                   ((COLS - 80) - strlen(msg)) / 2, "%s", msg);
//         wrefresh(ctx->progress_windows[3]);
//     }
// }
//
