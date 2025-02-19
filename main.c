#include "src/views/views.h"
#include <locale.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <string.h>

int EXIT_SUCCESS = 0;

int main(void)
{
    setlocale(LC_ALL, "");

    initscr();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, -1, -1);
    init_pair(3, COLOR_RED, -1);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    WINDOW *welcome_screen = newwin(rows, cols, 0, 0);
    draw_border(welcome_screen, 2, 0);
    wattron(welcome_screen, COLOR_PAIR(1) | A_BOLD);
    char *msg = "Welcome to NETWORKING TUTOR";
    char *msg2 = "Press any key to proceed";
    mvwprintw(welcome_screen, (rows / 2) - 1, (cols - strlen(msg)) / 2, msg,
              rows, cols);

    mvwprintw(welcome_screen, (rows / 2), (cols - strlen(msg2)) / 2, msg2, rows,
              cols);
    wattroff(welcome_screen, COLOR_PAIR(1) | A_BOLD);
    curs_set(0);

    sqlite3 *db = create_database(welcome_screen);
    // seed_courses_data(db, welcome_screen);
    wrefresh(welcome_screen);

    wgetch(welcome_screen);
    delwin(welcome_screen);
    clear();
    refresh();

    create_start_screen(db);

    sqlite3_close(db);
    endwin();

    return EXIT_SUCCESS;
}
