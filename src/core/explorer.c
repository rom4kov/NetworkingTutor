#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <ncurses.h>

void move_to_next_entry(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    f_tree->curr_entry_nr++;
    f_tree->current_entry = f_tree->current_entry->next;
    werase(*explorer_window);
    create_file_tree(explorer_window, f_tree);
    focus_window(explorer_window, 3, "Explorer");
    doupdate();
}

void move_to_prev_entry(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    f_tree->curr_entry_nr--;
    f_tree->current_entry = f_tree->current_entry->prev;
    werase(*explorer_window);
    create_file_tree(explorer_window, f_tree);
    focus_window(explorer_window, 3, "Explorer");
    doupdate();
}

void open_or_close_dir(FILE_TREE *f_tree, WINDOW **explorer_window)
{
    char *curr_path = return_trimmed(f_tree->current_entry->path);

    if (f_tree->current_entry->state == 'c')
    {
        f_tree->current_entry->state = 'o';
        open_sub_directory(curr_path, f_tree);
    }
    else if (f_tree->current_entry->state == 'o')
    {
        f_tree->current_entry->state = 'c';
        close_sub_directory(f_tree->current_entry,
                            f_tree->current_entry->num_of_open_entries, f_tree);
    }
    werase(*explorer_window);
    create_file_tree(explorer_window, f_tree);

    focus_window(explorer_window, 3, "Explorer");
    doupdate();
}

void open_file_from_explorer(APP_CONTEXT *ctx, bool *new_file_form_active)
{
    deallocate_buffer(ctx->t_buffer);
    ctx->t_buffer = initialize_buffer();
    ctx->scroll_offset = 0;
    if (ctx->file && ctx->file->_fileno > 0)
        fclose(ctx->file);
    open_file(ctx);

    *new_file_form_active = false;
    ctx->explorer_mode = false;
    ctx->editor_mode = true;
    ctx->active_window_idx = 2;
    focus_window(&ctx->course_windows[1], 2, "Explorer");
    focus_window(&ctx->course_windows[2], 3, "Editor");
    curs_set(2);

    wmove(ctx->edit_window, 0, 0);
    wnoutrefresh(ctx->course_windows[1]);
    wnoutrefresh(ctx->line_num_win);
    wnoutrefresh(ctx->course_windows[2]);
    wnoutrefresh(ctx->edit_window);
    doupdate();
}

void update_file_explorer(APP_CONTEXT *ctx)
{
    werase(ctx->course_windows[1]);
    deallocate_file_tree(ctx->file_tree);
    ctx->file_tree = initialize_file_tree();
    ctx->file_tree->prev_dir = initialize_dir_entry();
    create_file_tree(&ctx->course_windows[1], ctx->file_tree);
    focus_window(&ctx->course_windows[1], 3, "Explorer");
    doupdate();
}
