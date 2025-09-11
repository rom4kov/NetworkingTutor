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
#include <panel.h>
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
    ctx->num_of_courses = get_num_of_courses(ctx->db);
    ctx->courses = get_course_data(ctx->db, ctx->num_of_courses);
    ctx->file = NULL;
    ctx->file_tree = initialize_file_tree();
    ctx->file_tree->prev_dir = initialize_dir_entry();
    ctx->t_buffer = initialize_buffer();
    ctx->filename = (char *)calloc(30, sizeof(char));
    ctx->curr_file_path = (char *)calloc(30, sizeof(char));
    ctx->running = true;

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

    ctx->rp_state->curr_section = 0;
    ctx->rp_state->curr_item = 2;
    ctx->rp_state->items_to_print = 1;
    ctx->rp_state->curr_offset = 0;
    ctx->rp_state->scroll_offset = 0;
    ctx->rp_state->it_buffer = initialize_it_buffer();
    ctx->rp_state->course_progress =
        (int *)calloc(MAX_COURSE_SECTIONS, sizeof(int));
    ctx->rp_state->completed_sections =
        (bool *)calloc(MAX_COURSE_SECTIONS, sizeof(bool));
    ctx->rp_state->total_section_items =
        (int *)calloc(MAX_COURSE_SECTIONS, sizeof(int));
    ctx->rp_state->total_course_sections = 0;

    ctx->user_form_fields = false;

    ctx->shell = calloc(1, sizeof(SHELL));
    ctx->shell->terminal_active = false;
    ctx->shell->executable_running = false;
    ctx->shell->stop_executable = false;
    ctx->shell->curr_buf_idx = 0;
    ctx->shell->term_buffer = initialize_buffer();
    ctx->shell->term_buffer->num_of_lines = 1;
    ctx->shell->term_buffer->first_line = initialize_line();
    ctx->shell->term_buffer->first_line->buf_[0] = '>';
    ctx->shell->term_buffer->first_line->buf_[1] = ' ';
    ctx->shell->term_buffer->first_line->buf_[2] = '\0';
    ctx->shell->term_buffer->current_line = ctx->shell->term_buffer->first_line;
    ctx->shell->term_buffer->current_col = 2;
    ctx->shell->term_buffer->scroll_offset = 0;
    ctx->shell->cwd = "";
    ctx->shell->cwd_allocated = false;
    ctx->shell->home_dir = get_cwd();

    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_courses_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_sections_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_completed_courses_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_streaks_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_ascii_art_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/sections.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/0_intro.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/1_foundations.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/2_getaddrinfo.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/3_socket.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/4_bind.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/5_listen_and_accept.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/6_recv_and_send.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_users_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_progress_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/update_http_logo.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/create_tasks_table.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_server/tasks.sql");
    //
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_parser/sections.sql");
    // seed_courses_data(ctx->db, ctx->greeter_windows[0],
    //                   "SQL/courses/http_parser/0_intro.sql");

    ESCDELAY = 100;

    create_greeter_screen(ctx);
    ctx->active_window = ctx->greeter_windows[1];

    while (ctx->running)
    {
        if (ctx->greeter_needs_redraw)
        {
            if (!ctx->first_greeter_draw)
            {
                endwin();
                refresh();
                if (ctx->greeter_windows[0] != NULL)
                {
                    delwin(ctx->greeter_windows[0]);
                }
            }
            ctx->start_view_active = false;
            ctx->start_needs_redraw = false;
            create_greeter_screen(ctx);

            ctx->greeter_needs_redraw = false;
            ctx->first_greeter_draw = false;
            wrefresh(ctx->greeter_windows[0]);
        }
        if (ctx->start_needs_redraw)
        {
            if (!ctx->first_start_draw)
            {
                endwin();
                refresh();
                // for (int i = 0; i < START_WINDOW_COUNT; i++)
                // {
                //     if (ctx->start_windows[i] != NULL)
                //     {
                //         delwin(ctx->start_windows[i]);
                //     }
                // }
            }
            // deallocate_it_buffer(ctx->intro_buffer);
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
                // for (int i = 0; i < COURSE_WINDOW_COUNT; i++)
                // {
                //     if (ctx->course_windows[i] != NULL)
                //     {
                //         delwin(ctx->course_windows[i]);
                //     }
                // }
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
                // for (int i = 0; i < 4; i++)
                // {
                //     if (ctx->progress_windows[i] != NULL)
                //     {
                //         delwin(ctx->progress_windows[i]);
                //     }
                // }
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
                // for (int i = 0; i < 3; i++)
                // {
                //     if (ctx->all_courses_windows[i] != NULL)
                //     {
                //         delwin(ctx->all_courses_windows[i]);
                //     }
                // }
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
                // for (int i = 0; i < 3; i++)
                // {
                //     if (ctx->keybindings_windows[i] != NULL)
                //     {
                //         delwin(ctx->keybindings_windows[i]);
                //     }
                // }
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

    sqlite3_db_release_memory(ctx->db);

    FILE *log_file = fopen("sqlite_log_file.txt", "a");
    sqlite3_stmt *stmt;
    while ((stmt = sqlite3_next_stmt(ctx->db, NULL)) != NULL) {
        fprintf(stderr, "Leaked stmt: %s\n", sqlite3_sql(stmt));
        const char *sql_msg = sqlite3_sql(stmt);
        fwrite(sql_msg, strlen(sql_msg), 1, log_file);
        sqlite3_finalize(stmt);
    }

    int rc = sqlite3_close(ctx->db);
    if (rc != SQLITE_OK) {
        const char *err_msg = sqlite3_errmsg(ctx->db);
        fwrite(err_msg, strlen(err_msg), 1, log_file);
    }
    fclose(log_file);

    curs_set(1);

    free_memory_for_exit(ctx);

    endwin();

    exit_curses(0);

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
