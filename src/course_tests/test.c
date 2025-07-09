#include "../core/core.h"
#include "../models/models.h"
#include "01_http_server/http_server_tests.h"

#include "unistd.h"
#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <curses.h>
#include <ncurses.h>

int perform_tests(APP_CONTEXT *ctx)
{
    CU_ErrorCode ec = CU_initialize_registry();
    if (ec != CUE_SUCCESS)
    {
        return 1;
    }

    CU_pSuite sp = CU_add_suite("http_server_test_suite_01", NULL, NULL);
    ec = CU_get_error();
    if (ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(sp, "answers file exists",
                (CU_TestFunc)test_if_answers_file_exists);
    ec = CU_get_error();
    if (ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(sp, "answers file contains correct answers",
                (CU_TestFunc)test_if_answers_file_contains_correct_answers);
    ec = CU_get_error();
    if (ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_basic_set_mode(CU_BRM_SILENT);

    int saved_stdout = dup(STDOUT_FILENO);
    suppress_stdout();

    ec = CU_basic_run_tests();

    restore_stdout(saved_stdout);

    if (ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    const CU_pRunSummary run_sum = CU_get_run_summary();
    wattron(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->rp_state->inner_win, 0, 2, "%s", "TEST RESULTS");
    wattroff(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->rp_state->inner_win, 2, 2, "Suites run: %i",
              run_sum->nSuitesRun);
    mvwprintw(ctx->rp_state->inner_win, 3, 2, "Suites failed: %i",
              run_sum->nSuitesFailed);
    mvwprintw(ctx->rp_state->inner_win, 4, 2, "Tests run: %i",
              run_sum->nTestsRun);
    mvwprintw(ctx->rp_state->inner_win, 5, 2, "Tests failed: %i",
              run_sum->nTestsFailed);
    mvwprintw(ctx->rp_state->inner_win, 6, 2, "Asserts: %i", run_sum->nAsserts);
    mvwprintw(ctx->rp_state->inner_win, 7, 2, "Asserts failed: %i",
              run_sum->nAssertsFailed);
    mvwprintw(ctx->rp_state->inner_win, 8, 2, "Failure records: %i",
              run_sum->nFailureRecords);

    CU_pFailureRecord fail_rec = CU_get_failure_list();

    wattron(ctx->rp_state->inner_win, A_UNDERLINE);
    mvwprintw(ctx->rp_state->inner_win, 11, 2, "%s", "Failure record");
    wattroff(ctx->rp_state->inner_win, A_UNDERLINE);

    int i = 13;
    while (fail_rec != NULL)
    {
        mvwprintw(ctx->rp_state->inner_win, i, 2, "Line number: %i",
                  fail_rec->uiLineNumber);
        mvwprintw(ctx->rp_state->inner_win, i + 1, 2, "Filename: %s",
                  fail_rec->strFileName);
        mvwprintw(ctx->rp_state->inner_win, i + 2, 2, "Condition: %s",
                  fail_rec->strCondition);
        mvwprintw(ctx->rp_state->inner_win, i + 3, 2, "Test name: %s",
                  fail_rec->pTest->pName);
        mvwprintw(ctx->rp_state->inner_win, i + 4, 2, "Suite name: %s",
                  fail_rec->pSuite->pName);
        fail_rec = fail_rec->pNext;
        i += 6;
    }

    wrefresh(ctx->rp_state->inner_win);

    CU_cleanup_registry();
    return 0;
}
