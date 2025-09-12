#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_keybindings_view(APP_CONTEXT *ctx)
{
    ctx->keybindings_windows[1] = create_keybindings_screen();
    ctx->keybindings_windows[0] = create_navigation_window(ctx);
    ctx->keybindings_windows[2] = create_keybindings_container(ctx);

    wnoutrefresh(ctx->keybindings_windows[0]);
    wnoutrefresh(ctx->keybindings_windows[1]);
    wnoutrefresh(ctx->keybindings_windows[2]);
    doupdate();
}

WINDOW *create_keybindings_screen(void)
{
    WINDOW *keybindings_screen = newwin(LINES, COLS, 0, 0);
    draw_progress_border(keybindings_screen, 2, 1);
    return keybindings_screen;
}

WINDOW *create_keybindings_container(APP_CONTEXT *ctx)
{
    WINDOW *keybindings_container =
        derwin(ctx->keybindings_windows[1], LINES - 10, COLS - 16, 6, 8);

    char *keybindings_ascii = get_ascii_art(ctx->db, "keybindings");
    wattron(keybindings_container, A_BOLD);
    mvwprintw(keybindings_container, 0, 0, "%s", keybindings_ascii);
    wattroff(keybindings_container, A_BOLD);
    free(keybindings_ascii);

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 7, 0, "%s", "All views");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 9, 0, "%s",
              "Move between windows: Arrow keys ← ↑ → ↓");
    mvwprintw(keybindings_container, 11, 0, "%s",
              "Activate a window: move to window and press ENTER");
    mvwprintw(keybindings_container, 13, 0, "%s", "Deactivate a window: F1");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 16, 0, "%s", "Navbar");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 18, 0, "%s",
              "Go to different screen: ← → and press Enter");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 21, 0, "%s", "Start view");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 23, 0, "%s",
              "Go back to greeter screen: press Enter on logo window");
    mvwprintw(keybindings_container, 25, 0, "%s",
              "Go to a course: move to course card and press Enter");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 28, 0, "%s", "Editor window");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 30, 0, "%s",
              "Move cursor: Arrow keys ← ↑ → ↓");
    mvwprintw(keybindings_container, 32, 0, "%s",
              "Move to start of line: Home key");
    mvwprintw(keybindings_container, 34, 0, "%s",
              "Move to end of line: End key");
    mvwprintw(keybindings_container, 36, 0, "%s", "Save file: F10");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 7, 65, "%s", "Explorer window");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 9, 65, "%s",
              "Move up and down: up and down keys ↑ ↓");
    mvwprintw(keybindings_container, 11, 65, "%s",
              "Open file or directory: press Enter when on item");
    mvwprintw(keybindings_container, 13, 65, "%s",
              "Create new file: move to directory where you");
    mvwprintw(keybindings_container, 14, 65, "%s",
              "                 want to create the file and");
    mvwprintw(keybindings_container, 15, 65, "%s",
              "                 press 'a' key");
    mvwprintw(keybindings_container, 17, 65, "%s",
              "Create new directory: 'm' key");
    mvwprintw(keybindings_container, 19, 65, "%s",
              "Rename a file or directory: 'r' key");
    mvwprintw(keybindings_container, 21, 65, "%s",
              "Delete file or directory: 'd' key");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 24, 65, "%s",
              "Course instructions window");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 26, 65, "%s",
              "Scroll up and down: up and down keys ↑ ↓");
    mvwprintw(keybindings_container, 28, 65, "%s",
              "Move between completed course sections: '<' and '>' keys");
    mvwprintw(keybindings_container, 30, 65, "%s",
              "Show next paragraph / text item: space key");
    mvwprintw(keybindings_container, 32, 65, "%s",
              "Show separate task if there is one: 't' key");
    mvwprintw(keybindings_container, 34, 65, "%s",
              "Submit your the code you've written: 's' key");
    mvwprintw(keybindings_container, 36, 65, "%s",
              "Complete current section and go to next one: ENTER");

    wattron(keybindings_container, A_UNDERLINE | A_BOLD);
    mvwprintw(keybindings_container, 7, 135, "%s", "Terminal window");
    wattroff(keybindings_container, A_UNDERLINE | A_BOLD);

    mvwprintw(keybindings_container, 9, 135, "%s",
              "To toggle the terminal on / off: press 't' when on editor ");
    mvwprintw(keybindings_container, 10, 135, "%s",
              "                                 or terminal window");
    mvwprintw(keybindings_container, 12, 135, "%s",
              "Press any key to type commands in the terminal");
    mvwprintw(keybindings_container, 14, 135, "%s",
              "Press ENTER to run the command");
    mvwprintw(keybindings_container, 16, 135, "%s",
              "Scroll up and down the terminal: up and down keys ↑ ↓");
    mvwprintw(keybindings_container, 18, 135, "%s",
              "Stop a running executable: F2");
    mvwprintw(keybindings_container, 20, 135, "%s",
              "Get out of terminal window: F1");

    return keybindings_container;
}
