#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>

void free_memory_for_exit(APP_CONTEXT *ctx)
{
    if (ctx->greeter_view_active)
    {
        free(ctx->file_tree->prev_dir->name);
        free(ctx->file_tree->prev_dir->path);
        free(ctx->file_tree->prev_dir);
        deallocate_file_tree(ctx->file_tree);
        deallocate_buffer(ctx->t_buffer);
        deallocate_buffer(ctx->shell->term_buffer);
        deallocate_it_buffer(ctx->rp_state->it_buffer);
        deallocate_greeter_memory(ctx);
    }
    else if (ctx->start_view_active)
    {
        cleanup_start_for_exit(ctx);
    }
    else if (ctx->course_view_active)
    {
        cleanup_course_for_exit(ctx);
    }
    else if (ctx->all_courses_view_active)
    {
        cleanup_all_courses_for_exit(ctx);
    }
    else if (ctx->progress_view_active)
    {
        cleanup_progress_for_exit(ctx);
    }
    else if (ctx->keybindings_view_active)
    {
        cleanup_keybindings_for_exit(ctx);
    }
    // free(ctx->rp_state->s_metadata->title);
    // free(ctx->rp_state->s_metadata);

    CU_cleanup_registry();

    free(ctx->shell->home_dir);
    if (ctx->shell->cwd_allocated)
        free(ctx->shell->cwd);
    free(ctx->shell);
    free(ctx->filename);
    free(ctx->curr_file_path);
    // free(ctx->user_data);

    for (int i = 0; i < ctx->num_of_courses; i++) {
        free(ctx->courses[i].name);
        free(ctx->courses[i].short_desc);
        free(ctx->courses[i].ascii_logo);
    }
    free(ctx->courses);

    // free(ctx->file_tree);
    free(ctx->rp_state->course_progress);
    free(ctx->rp_state->completed_sections);
    free(ctx->rp_state->total_section_items);
    // free(ctx->rp_state->it_buffer);
    free(ctx->rp_state);
    ctx->db = NULL;
    free(ctx);
}

void free_memory_for_switch(APP_CONTEXT *ctx)
{
    if (ctx->greeter_view_active)
    {
        deallocate_file_tree(ctx->file_tree);
        deallocate_buffer(ctx->t_buffer);
        deallocate_buffer(ctx->shell->term_buffer);
        deallocate_it_buffer(ctx->rp_state->it_buffer);
        deallocate_greeter_memory(ctx);
        ctx->greeter_view_active = false;
    }
    else if (ctx->start_view_active)
    {
        cleanup_start_for_switch(ctx);
        ctx->start_view_active = false;
    }
    else if (ctx->course_view_active)
    {
        cleanup_course_for_switch(ctx);
        ctx->course_view_active = false;
    }
    else if (ctx->all_courses_view_active)
    {
        cleanup_all_courses_for_switch(ctx);
        ctx->all_courses_view_active = false;
    }
    else if (ctx->progress_view_active)
    {
        cleanup_progress_for_switch(ctx);
        ctx->progress_view_active = false;
    }
    else if (ctx->keybindings_view_active)
    {
        cleanup_keybindings_for_switch(ctx);
        ctx->keybindings_view_active = false;
    }
}

void cleanup_init_state(APP_CONTEXT *ctx)
{
    deallocate_buffer(ctx->t_buffer);
    deallocate_buffer(ctx->shell->term_buffer);
    if (ctx->start_view_active)
    {
        deallocate_it_buffer(ctx->intro_buffer);
    }
    deallocate_it_buffer(ctx->rp_state->it_buffer);
    deallocate_file_tree(ctx->file_tree);

    free(ctx->user_data->name);
    free(ctx->user_data->created_at);
    free(ctx->user_data);
}

void cleanup_nav_menu(APP_CONTEXT *ctx)
{
    unpost_menu(ctx->start_menu);
    free_menu(ctx->start_menu);
    for (int i = 0; i < 6; i++)
    {
        free_item(ctx->nav_menu_items[i]);
    }
    free(ctx->nav_menu_items);
}

void free_section_data(APP_CONTEXT *ctx)
{
    if (ctx->rp_state->course_section_data)
    {
        for (int i = 0;
             i <
             ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section];
             i++)
        {
            free(ctx->rp_state->course_section_data[i].content_title);
            free(ctx->rp_state->course_section_data[i].content);
        }
        free(ctx->rp_state->course_section_data);
    }

    if (ctx->rp_state->s_metadata)
    {
        free(ctx->rp_state->s_metadata->title);
        free(ctx->rp_state->s_metadata);
    }
}

void cleanup_start_for_exit(APP_CONTEXT *ctx)
{
    cleanup_init_state(ctx);

    cleanup_nav_menu(ctx);

    for (int i = 1; i < START_WINDOW_COUNT; i++)
    {
        delwin(ctx->start_windows[i]);
    }
}

void cleanup_start_for_switch(APP_CONTEXT *ctx)
{
    deallocate_it_buffer(ctx->intro_buffer);

    cleanup_nav_menu(ctx);

    for (int i = 0; i < START_WINDOW_COUNT; i++)
    {
        wclear(ctx->start_windows[i]);
        wnoutrefresh(ctx->start_windows[i]);
        delwin(ctx->start_windows[i]);
        ctx->start_windows[i] = NULL;
    }
}

void cleanup_course_for_exit(APP_CONTEXT *ctx)
{
    if (!ctx->rp_state->showing_end_of_course_page)
    {
        free_section_data(ctx);
    }

    cleanup_init_state(ctx);

    cleanup_nav_menu(ctx);

    for (int i = 0; i < COURSE_WINDOW_COUNT; i++)
    {
        delwin(ctx->course_windows[i]);
    }
}

void cleanup_course_for_switch(APP_CONTEXT *ctx)
{
    if (!ctx->rp_state->showing_end_of_course_page)
    {
        free_section_data(ctx);
    }

    cleanup_nav_menu(ctx);

    for (int i = 0; i < COURSE_WINDOW_COUNT; i++)
    {
        wclear(ctx->course_windows[i]);
        wnoutrefresh(ctx->course_windows[i]);
        delwin(ctx->course_windows[i]);
        ctx->course_windows[i] = NULL;
    }
    doupdate();
}

void cleanup_all_courses_for_exit(APP_CONTEXT *ctx)
{
    cleanup_init_state(ctx);

    cleanup_nav_menu(ctx);

    for (int i = 0; i < ALL_COURSES_WINDOW_COUNT; i++)
    {
        delwin(ctx->all_courses_windows[i]);
    }
}

void cleanup_all_courses_for_switch(APP_CONTEXT *ctx)
{
    cleanup_nav_menu(ctx);

    for (int i = 0; i < ALL_COURSES_WINDOW_COUNT; i++)
    {
        wclear(ctx->all_courses_windows[i]);
        wnoutrefresh(ctx->all_courses_windows[i]);
        delwin(ctx->all_courses_windows[i]);
    }
    doupdate();
}

void cleanup_progress_for_exit(APP_CONTEXT *ctx)
{
    cleanup_init_state(ctx);

    cleanup_nav_menu(ctx);

    unpost_form(ctx->user_form);
    free_form(ctx->user_form);
    free_field(ctx->user_form_field[0]);

    for (int i = 0; i < PROGRESS_WINDOW_COUNT; i++)
    {
        delwin(ctx->progress_windows[i]);
    }
}

void cleanup_progress_for_switch(APP_CONTEXT *ctx)
{
    cleanup_nav_menu(ctx);

    unpost_form(ctx->user_form);
    free_form(ctx->user_form);
    free_field(ctx->user_form_field[0]);

    for (int i = 0; i < PROGRESS_WINDOW_COUNT; i++)
    {
        wclear(ctx->progress_windows[i]);
        wnoutrefresh(ctx->progress_windows[i]);
        delwin(ctx->progress_windows[i]);
    }
    doupdate();
}

void cleanup_keybindings_for_exit(APP_CONTEXT *ctx)
{
    cleanup_init_state(ctx);

    cleanup_nav_menu(ctx);

    for (int i = 0; i < KEYBINDINGS_WINDOW_COUNT; i++)
    {
        delwin(ctx->keybindings_windows[i]);
    }
}

void cleanup_keybindings_for_switch(APP_CONTEXT *ctx)
{
    cleanup_nav_menu(ctx);

    for (int i = 0; i < KEYBINDINGS_WINDOW_COUNT; i++)
    {
        wclear(ctx->keybindings_windows[i]);
        wnoutrefresh(ctx->keybindings_windows[i]);
        delwin(ctx->keybindings_windows[i]);
    }
    doupdate();
}
