#include "src/core/core.h"
#include "src/data/data_access_layer.h"
#include "src/models/models.h"
#include "src/views/views.h"
#include <curses.h>
#include <locale.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

#define COLOR_GREY 16
#define COLOR_ORANGE 17
#define COLOR_DARKGREY 18
#define START_WINDOW_COUNT 6
#define COURSE_WINDOW_COUNT 4

int main(void)
{
    setlocale(LC_ALL, "");

    initscr();

    initialize_colors();

    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    curs_set(0);

    WINDOW *welcome_screen = create_welcome_screen();

    sqlite3 *db = create_database(welcome_screen);
    // seed_courses_data(db, welcome_screen);

    int rc = wgetch(welcome_screen);
    if (rc == KEY_RESIZE)
    {
        // wresize(welcome_screen, LINES, COLS);
        wclear(welcome_screen);
        wrefresh(welcome_screen);
    }

    delwin(welcome_screen);
    clear();
    refresh();

    APP_CONTEXT *ctx = (APP_CONTEXT *)malloc(sizeof(APP_CONTEXT));

    memset(ctx, 0, sizeof(APP_CONTEXT));
    ctx->courses = get_course_data(db);
    ctx->file_tree = initialize_file_tree();
    ctx->t_buffer = initialize_buffer();
    ctx->filename = (char *)calloc(30, sizeof(char));
    ctx->running = true;
    ctx->start_needs_redraw = true;
    ctx->course_needs_redraw = false;
    ctx->first_start_draw = true;
    ctx->first_course_draw = true;
    ctx->start_view_active = true;
    ctx->course_view_active = false;

    // WINDOW *start_windows[START_WINDOW_COUNT];
    // WINDOW *course_windows[COURSE_WINDOW_COUNT];
    // WINDOW *line_num_win;
    // WINDOW *edit_window;
    // COURSE *courses = get_course_data(db);

    // int active_window = 0;
    // MENU *start_menu = NULL;
    // MENU *explorer_menu = NULL;
    // ITEM **menu_items = NULL;
    // ITEM *curr_item;
    // FILE_TREE *file_tree = initialize_file_tree();

    // bool running = true;
    // bool start_needs_redraw = true;
    // bool first_start_draw = true;
    // bool first_course_draw = true;
    // bool start_view_active = true;
    // bool course_needs_redraw = false;
    // bool course_view_active = false;

    // FILE *file = NULL;

    // TEXT_BUFFER *t_buffer = initialize_buffer();

    // bool editor_mode = false;
    // bool explorer_mode = false;

    // int y, x;
    // int scroll_offset = 0;
    // int lines_to_print;
    int curr_line;
    int curr_col;

    char *filename = (char *)calloc(30, sizeof(char));

    ESCDELAY = 100;

    while (ctx->running)
    {
        if (ctx->start_needs_redraw)
        {
            if (!ctx->first_start_draw)
            {
                endwin();
                refresh();
                for (int i = 0; i < START_WINDOW_COUNT; i++)
                {
                    if (ctx->start_windows[i] != NULL)
                    {
                        delwin(ctx->start_windows[i]);
                    }
                }
            }
            create_start_screen(ctx->start_windows, &ctx->active_window, &ctx->start_menu,
                                ctx->courses, db);
            ctx->start_needs_redraw = false;
            ctx->first_start_draw = false;
        }
        else if (ctx->course_needs_redraw)
        {
            if (!ctx->first_course_draw)
            {
                endwin();
                refresh();
                for (int i = 0; i < COURSE_WINDOW_COUNT; i++)
                {
                    if (ctx->course_windows[i] != NULL)
                    {
                        delwin(ctx->course_windows[i]);
                    }
                }
            }
            create_course_view(ctx);
            if (ctx->file && ctx->file->_fileno > 0)
            {
                fclose(ctx->file);
                curr_line = ctx->t_buffer->curr_line_nr < LINES - 8
                                ? ctx->t_buffer->curr_line_nr
                                : ctx->scroll_offset + LINES - 8;
                curr_col = ctx->t_buffer->current_col;
                // deallocate_buffer(t_buffer);
                // t_buffer = initialize_buffer();
                ctx->t_buffer->curr_line_nr = curr_line;
                open_file(filename, &ctx->file, ctx->t_buffer, &ctx->line_num_win,
                          &ctx->course_windows[2], &ctx->edit_window, &ctx->scroll_offset,
                          &ctx->lines_to_print);
                ctx->t_buffer->curr_line_nr = curr_line;
                ctx->t_buffer->current_col = curr_col;
                ctx->t_buffer->current_line = ctx->t_buffer->first_line;
                for (int i = 0; i < curr_line; i++)
                {
                    ctx->t_buffer->current_line = ctx->t_buffer->current_line->next;
                }
                ctx->explorer_mode = false;
                ctx->editor_mode = true;
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[0], 2, "Explorer");
                focus_window(&ctx->course_windows[2], 3, "Editor");
                curs_set(2);
                wmove(ctx->edit_window, curr_line - ctx->scroll_offset, curr_col);
                wnoutrefresh(ctx->course_windows[1]);
                wnoutrefresh(ctx->line_num_win);
                wnoutrefresh(ctx->course_windows[2]);
                wnoutrefresh(ctx->edit_window);
                doupdate();
            }
            ctx->course_needs_redraw = false;
            ctx->first_course_draw = false;
        }

        int key = getch();

        switch (key)
        {
            case KEY_RESIZE:
                if (ctx->start_view_active)
                {
                    ctx->start_needs_redraw = true;
                }
                else if (ctx->course_view_active)
                {
                    ctx->course_needs_redraw = true;
                }
                break;
            case 27:
                ctx->running = false;
                break;
            default:
                if (ctx->start_view_active)
                {
                    handle_start_input(&key, start_windows, &start_view_active,
                                       &course_view_active, &start_needs_redraw,
                                       &course_needs_redraw, &active_window,
                                       &start_menu, courses, &db);
                }
                else if (ctx->course_view_active)
                {
                    handle_course_input(
                        &key, course_windows, &line_num_win, &edit_window,
                        &start_view_active, &course_view_active, &active_window,
                        &start_needs_redraw, &start_menu, &curr_item,
                        &explorer_menu, file_tree, &menu_items, &filename, &explorer_mode,
                        &editor_mode, &file, t_buffer, &scroll_offset,
                        &lines_to_print, &y, &x);
                }
        }
    }

    sqlite3_close(db);

    curs_set(1);

    endwin();

    return EXIT_SUCCESS;
}

void initialize_colors()
{
    start_color();
    use_default_colors();

    init_color(COLOR_GREY, 800, 800, 800);

    init_pair(1, COLOR_GREY, -1);
    init_pair(2, -1, -1);
    init_pair(3, COLOR_RED, -1);
    // if (can_change_color())
    //      init_color(COLOR_WHITE, 195, 225, 225);
    init_pair(4, COLOR_GREEN, -1);
    init_pair(5, COLOR_YELLOW, -1);
    init_pair(6, COLOR_CYAN, -1);
    init_pair(7, COLOR_BLUE, -1);
    init_pair(8, COLOR_MAGENTA, -1);

    init_color(COLOR_ORANGE, 965, 549, 249);

    init_pair(9, COLOR_ORANGE, -1);

    init_color(COLOR_DARKGREY, 500, 500, 500);

    init_pair(10, COLOR_DARKGREY, -1);
}

WINDOW *create_welcome_screen()
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    WINDOW *welcome_screen = newwin(rows, cols, 0, 0);
    draw_border(welcome_screen, 2, 0);
    wattron(welcome_screen, COLOR_PAIR(3) | A_BOLD);
    char *msg = "Welcome to NETWORKING TUTOR";
    char *msg2 = "Press any key to proceed";
    mvwprintw(welcome_screen, (rows / 2) - 1, (cols - strlen(msg)) / 2, msg,
              rows, cols);

    mvwprintw(welcome_screen, (rows / 2), (cols - strlen(msg2)) / 2, msg2, rows,
              cols);
    wattroff(welcome_screen, COLOR_PAIR(3) | A_BOLD);
    wrefresh(welcome_screen);

    return welcome_screen;
}
