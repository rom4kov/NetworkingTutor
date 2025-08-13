#include "../models/models.h"
#include "../views/views.h"
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <CUnit/CUnit.h>


void *print_press_enter_after_sec_compl(void *arg)
{
    sleep(3);
    APP_CONTEXT *ctx = (APP_CONTEXT *)arg;
    create_progress_window(ctx);
    if (ctx->rp_state->curr_section + 1 < ctx->rp_state->total_course_sections)
    {
        char *press_enter = "Press ENTER to go to next section";
        mvwprintw(ctx->rp_state->right_panel, LINES - 5,
                  (ctx->rp_state->window_width - strlen(press_enter)) / 2, "%s",
                  press_enter);
    }
    else
{
        char *press_enter = "Press ENTER to finish the course";
        mvwprintw(ctx->rp_state->right_panel, LINES - 5,
                  (ctx->rp_state->window_width - strlen(press_enter)) / 2, "%s",
                  press_enter);
    }

    mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s",
              "< Back to section");
    wrefresh(ctx->rp_state->right_panel);
    return NULL;
}

void print_section_or_task_compl_msg(APP_CONTEXT *ctx, CU_pRunSummary rs)
{
    char blank_line[ctx->rp_state->window_width - 2];
    memset(blank_line, 32, sizeof(blank_line));
    memset(&blank_line[sizeof(blank_line) - 1], '\0', 1);
    mvwprintw(ctx->course_windows[4], 1, 1, "%s", blank_line);

    char *msg = NULL;
    if (rs && rs->nTestsFailed == 0)
    {
        msg = "TASK COMPLETED!";
    }
    else if (rs == NULL)
    {
        msg = "SECTION COMPLETED!";
    }
    wattron(ctx->course_windows[4], COLOR_PAIR(4) | A_BOLD);
    mvwprintw(ctx->course_windows[4], 1,
              (ctx->rp_state->window_width - strlen(msg)) / 2, "%s", msg);
    wattroff(ctx->course_windows[4], COLOR_PAIR(4) | A_BOLD);
    wrefresh(ctx->course_windows[4]);

    pthread_t delayed_msg;
    pthread_create(&delayed_msg, NULL, print_press_enter_after_sec_compl, ctx);
    pthread_detach(delayed_msg);
}

