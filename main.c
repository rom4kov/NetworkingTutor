#include "ntutor.h"
#include "src/core/core.h"
#include "src/data/data_access_layer.h"
#include "src/models/models.h"
#include "src/views/start/start_menu.h"
#include "src/views/views.h"
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <curses.h>
#include <locale.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    setlocale(LC_ALL, "");

    initscr();

    initialize_colors();

    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    curs_set(0);

    APP_CONTEXT *ctx = (APP_CONTEXT *)malloc(sizeof(APP_CONTEXT));

    memset(ctx, 0, sizeof(APP_CONTEXT));

    ctx->db = create_database();
    ctx->rp_state = (RIGHT_PANEL_STATE *)malloc(sizeof(RIGHT_PANEL_STATE));
    ctx->courses = get_course_data(ctx->db);
    ctx->file = NULL;
    ctx->file_tree = initialize_file_tree();
    ctx->t_buffer = initialize_buffer();
    ctx->filename = (char *)calloc(30, sizeof(char));
    ctx->curr_file_path = (char *)calloc(30, sizeof(char));
    ctx->running = true;
    ctx->user_data = malloc(sizeof(USER_DATA));

    ctx->greeter_needs_redraw = false;
    ctx->start_needs_redraw = false;
    ctx->course_needs_redraw = false;
    ctx->progress_needs_redraw = false;
    ctx->all_courses_needs_redraw = false;

    ctx->first_start_draw = true;
    ctx->first_course_draw = true;

    ctx->greeter_view_active = true;
    ctx->start_view_active = false;
    ctx->course_view_active = false;
    ctx->progress_view_active = false;
    ctx->all_courses_view_active = false;

    ctx->current_course = malloc(64 * sizeof(char));
    ctx->rp_state->curr_section = 0;
    ctx->rp_state->curr_item = 2;
    ctx->rp_state->items_to_print = 1;
    ctx->rp_state->curr_offset = 0;
    ctx->rp_state->scroll_offset = 0;
    ctx->rp_state->it_buffer = initialize_it_buffer();
    ctx->rp_state->course_progress = (int *)calloc(32, sizeof(int));
    ctx->rp_state->completed_sections = (bool *)calloc(32, sizeof(bool));
    ctx->rp_state->total_section_items = (int *)calloc(32, sizeof(int));
    ctx->rp_state->total_course_sections = 0;

    ctx->user_form_fields = false;

    ctx->shell = calloc(1, sizeof(SHELL));
    ctx->shell->terminal_active = false;
    ctx->shell->executable_running = false;
    // bool *stop_exec = false;
    ctx->shell->stop_executable = false;
    // ctx->shell->stop_executable = stop_exec;
    ctx->shell->curr_buf_idx = 0;
    ctx->shell->term_buffer = initialize_buffer();
    ctx->shell->term_buffer->num_of_lines = 1;
    ctx->shell->term_buffer->first_line->buf_[0] = '>';
    ctx->shell->term_buffer->first_line->buf_[1] = ' ';
    ctx->shell->term_buffer->first_line->buf_[2] = '\0';
    ctx->shell->term_buffer->current_line = ctx->shell->term_buffer->first_line;
    ctx->shell->term_buffer->current_col = 2;
    ctx->shell->term_buffer->scroll_offset = 0;
    ctx->shell->cwd = "";
    ctx->shell->home_dir = get_cwd();

    // ctx->shell->term_buffer->first_line->buf_[0] = '>';
    // ctx->shell->term_buffer->first_line->buf_[1] = ' ';

    // refresh();

    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/create_courses_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/create_sections_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/create_completed_courses_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/create_streaks_table.sql");
    seed_courses_data(ctx->db, ctx->greeter_screen,
                      "SQL/create_ascii_art_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/sections.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/0_intro.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/1_foundations.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/2_getaddrinfo.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/3_socket.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/4_bind.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/5_listen_and_accept.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/6_recv_and_send.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/create_users_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/create_progress_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/update_http_logo.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/create_tasks_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_screen,
    //                   "SQL/courses/http_server/tasks.sql");

    ESCDELAY = 100;

    ctx->greeter_screen = create_greeter_screen(ctx);
    ctx->active_window = ctx->greeter_screen;
    keypad(ctx->greeter_screen, TRUE);
    wrefresh(ctx->greeter_screen);

    while (ctx->running)
    {
        if (ctx->greeter_needs_redraw)
        {
            if (!ctx->first_greeter_draw)
            {
                endwin();
                refresh();
                if (ctx->greeter_screen != NULL)
                {
                    delwin(ctx->greeter_screen);
                }
            }
            ctx->start_view_active = false;
            ctx->start_needs_redraw = false;
            ctx->greeter_screen = create_greeter_screen(ctx);
            ctx->active_window = ctx->greeter_screen;
            ctx->greeter_needs_redraw = false;
            ctx->first_greeter_draw = false;
            wrefresh(ctx->greeter_screen);
        }
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
            deallocate_it_buffer(ctx->intro_buffer);
            create_start_screen(ctx);
            ctx->active_window = ctx->start_windows[0];
            ctx->start_needs_redraw = false;
            ctx->first_start_draw = false;
            for (int i = 0; i < START_WINDOW_COUNT; i++)
                wnoutrefresh(ctx->start_windows[i]);
            doupdate();
        }
        else if (ctx->course_needs_redraw)
        {
            refresh();
            if (!ctx->first_course_draw)
            {
                if (ctx->rp_state->curr_section > 0)
                    ctx->rp_state->items_to_print = ctx->rp_state->curr_item;
                else
                    ctx->rp_state->items_to_print =
                        ctx->rp_state->curr_item - 1;

                ctx->rp_state->it_buffer = initialize_it_buffer();

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
            ctx->active_window = ctx->course_windows[0];
            if (ctx->file && ctx->file->_fileno > 0)
            {
                bool activate_editor = true;
                reopen_file(ctx, activate_editor);
            }
            wnoutrefresh(ctx->course_windows[0]);
            wnoutrefresh(ctx->course_windows[1]);
            wnoutrefresh(ctx->line_num_win);
            wnoutrefresh(ctx->course_windows[2]);
            wnoutrefresh(ctx->edit_window);
            wnoutrefresh(ctx->course_windows[4]);
            doupdate();
            ctx->course_needs_redraw = false;
            ctx->first_course_draw = false;
        }
        else if (ctx->progress_needs_redraw)
        {
            if (!ctx->first_progress_draw)
            {

                endwin();
                refresh();
                for (int i = 0; i < 4; i++)
                {
                    if (ctx->progress_windows[i] != NULL)
                    {
                        delwin(ctx->progress_windows[i]);
                    }
                }
            }
            create_progress_view(ctx);
            ctx->active_window = ctx->progress_windows[0];
            ctx->progress_needs_redraw = false;
            ctx->first_progress_draw = false;
            for (int i = 0; i < 3; i++)
                wnoutrefresh(ctx->progress_windows[i]);
            doupdate();
        }
        else if (ctx->all_courses_needs_redraw)
        {
            if (!ctx->first_all_courses_draw)
            {

                endwin();
                refresh();
                for (int i = 0; i < 3; i++)
                {
                    if (ctx->all_courses_windows[i] != NULL)
                    {
                        delwin(ctx->all_courses_windows[i]);
                    }
                }
            }
            create_all_courses_view(ctx);
            ctx->active_window = ctx->all_courses_windows[0];
            ctx->all_courses_needs_redraw = false;
            ctx->first_all_courses_draw = false;
            for (int i = 0; i < 3; i++)
                wnoutrefresh(ctx->all_courses_windows[i]);
            doupdate();
        }
        else if (ctx->keybindings_needs_redraw)
        {
            if (!ctx->first_keybindings_draw)
            {

                endwin();
                refresh();
                for (int i = 0; i < 3; i++)
                {
                    if (ctx->keybindings_windows[i] != NULL)
                    {
                        delwin(ctx->keybindings_windows[i]);
                    }
                }
            }
            create_keybindings_view(ctx);
            ctx->active_window = ctx->keybindings_windows[0];
            ctx->keybindings_needs_redraw = false;
            ctx->first_keybindings_draw = false;
            for (int i = 0; i < 3; i++)
                wnoutrefresh(ctx->keybindings_windows[i]);
            doupdate();
        }

        keypad(ctx->active_window, TRUE);
        ctx->key = wgetch(ctx->active_window);

        switch (ctx->key)
        {
            case KEY_RESIZE:
                if (ctx->greeter_view_active)
                {
                    ctx->greeter_needs_redraw = true;
                }
                else if (ctx->start_view_active)
                {
                    ctx->start_needs_redraw = true;
                }
                else if (ctx->course_view_active)
                {
                    ctx->course_needs_redraw = true;
                }
                else if (ctx->progress_view_active)
                {
                    ctx->progress_needs_redraw = true;
                }
                else if (ctx->all_courses_view_active)
                {
                    ctx->all_courses_needs_redraw = true;
                }
                else if (ctx->keybindings_view_active)
                {
                    ctx->keybindings_needs_redraw = true;
                }
                break;
            case 27:
                CU_cleanup_registry();
                ctx->running = false;
                break;
            default:
                if (ctx->greeter_view_active)
                {
                    handle_greeter_input(ctx);
                }
                else if (ctx->start_view_active)
                {
                    handle_start_input(ctx);
                }
                else if (ctx->course_view_active)
                {
                    handle_course_input(ctx);
                }
                else if (ctx->progress_view_active)
                {
                    handle_progress_input(ctx);
                }
                else if (ctx->all_courses_view_active)
                {
                    handle_nav_input(ctx);
                }
                else if (ctx->keybindings_view_active)
                {
                    handle_nav_input(ctx);
                }
        }
    }

    sqlite3_close(ctx->db);

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

    init_pair(11, COLOR_BLACK, -1);
    init_pair(12, COLOR_BLACK, COLOR_RED);
    init_pair(13, COLOR_BLACK, COLOR_GREEN);

    init_color(COLOR_TEALGREEN, 153, 616, 686);
    init_pair(14, COLOR_TEALGREEN, -1);

    init_color(COLOR_BERMUDA, 523, 851, 718);
    init_pair(15, COLOR_BERMUDA, -1);
}

MENU *create_greeter_menu(WINDOW *nav_window)
{
    const char *choices[] = {
        "   🛫 Start new learning path",
        " 👉 Continue where you left off",
        "     👤 Create new account",
        "          🚀 Progress",
        "            Settings",
        "            Shortcuts",
        "            🚪 Quit",
        (char *)NULL // Last element must be NULL
    };

    ITEM **menu_items = (ITEM **)calloc(8, sizeof(ITEM *));

    for (int i = 0; choices[i] != NULL; i++)
    {
        menu_items[i] = new_item(choices[i], "");
    }

    // Create the menu
    MENU *menu = new_menu(menu_items);
    set_menu_format(menu, 14, 1);
    set_menu_spacing(menu, 0, 2, 2);
    // mvwprintw(nav_window, 2, 3, "%i", COLS);

    // Set the window for the menu to be displayed inside left_inner_win
    set_menu_win(menu, nav_window);
    set_menu_sub(menu, derwin(nav_window, 28, COLS / 6, (LINES / 5) * 2 + 3,
                              (COLS - 35) / 2 - 1));
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > "); // Mark for the selected item

    // Post the menu (make it visible)
    post_menu(menu);

    // Refresh the left_inner_win window
    wrefresh(nav_window);
    return menu;
}

WINDOW *create_greeter_screen(APP_CONTEXT *ctx)
{
    WINDOW *greeter_screen = newwin(LINES, COLS, 0, 0);
    ctx->greeter_ascii_window =
        derwin(greeter_screen, 10, 71, LINES / 5, (COLS - 71) / 2 + 2);
    draw_border(greeter_screen, 2, 0);
    wattron(greeter_screen, COLOR_PAIR(3) | A_BOLD);
    char *msg = "Welcome to";
    char *msg2 = "󰒍 NetworkingTutor v0.0.1";
    mvwprintw(greeter_screen, (LINES / 5) - 1, (COLS - strlen(msg)) / 2, "%s",
              msg);
    wattron(ctx->greeter_ascii_window, A_BOLD);
    mvwprintw(ctx->greeter_ascii_window, 0, 0, "%s",
              get_ascii_art(ctx->db, "logo"));
    wattroff(ctx->greeter_ascii_window, A_BOLD);

    ctx->greeter_menu = create_greeter_menu(greeter_screen);

    wattroff(greeter_screen, COLOR_PAIR(3) | A_BOLD);

    wattron(greeter_screen, COLOR_PAIR(3));
    mvwprintw(greeter_screen, (LINES / 2) + 14, (COLS - strlen(msg2)) / 2 + 2,
              msg2, LINES, COLS);
    wattroff(greeter_screen, COLOR_PAIR(3));
    wrefresh(ctx->greeter_ascii_window);

    return greeter_screen;
}
