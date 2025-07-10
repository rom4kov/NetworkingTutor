#include "../core/core.h"
#include "../models/models.h"
#include "01_http_server/http_server_tests.h"

#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <curses.h>
#include <ncurses.h>
#include <unistd.h>

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

    CU_pRunSummary run_sum = CU_get_run_summary();
    print_run_summary(ctx, run_sum);

    CU_pFailureRecord fail_rec = CU_get_failure_list();
    print_failure_list(ctx, fail_rec);

    int i = 0;
    CU_pTestRegistry tr = CU_get_registry();
    CU_pSuite suite = CU_get_suite_by_name("http_server_test_suite_01", tr);
    CU_pTest test = suite->pTest;

    while (test != NULL) {
        // if (strcmp(test->pName, fail_rec->pTest->pName) == 0)
        // {
        // }
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(ctx->rp_state->inner_win, 15 + i, 2, "%s", test->pName);
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(4) | A_BOLD);
        test = test->pNext;
        i++;
    }

    wrefresh(ctx->rp_state->inner_win);

    CU_cleanup_registry();
    return 0;
}

void print_run_summary(APP_CONTEXT *ctx, CU_pRunSummary run_sum)
{
    wattron(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->rp_state->inner_win, 0, 2, "%s", "TEST RESULTS");
    wattroff(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->rp_state->inner_win, 2, 2, "Suites run: %i",
              run_sum->nSuitesRun);
    mvwprintw(ctx->rp_state->inner_win, 3, 2, "Suites failed: %i",
              run_sum->nSuitesFailed);
    mvwprintw(ctx->rp_state->inner_win, 4, 2, "Tests run: %i",
              run_sum->nTestsRun);

    // print how many tests failed, colored
    if (run_sum->nTestsFailed > 0)
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(4));
    mvwprintw(ctx->rp_state->inner_win, 5, 2, "Tests failed: %i",
              run_sum->nTestsFailed);
    if (run_sum->nTestsFailed > 0)
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(4));

    mvwprintw(ctx->rp_state->inner_win, 6, 2, "Asserts: %i", run_sum->nAsserts);

    // print how many asserts failed, colored
    if (run_sum->nAssertsFailed > 0)
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(4));
    mvwprintw(ctx->rp_state->inner_win, 7, 2, "Asserts failed: %i",
              run_sum->nAssertsFailed);
    if (run_sum->nAssertsFailed > 0)
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(4));

    mvwprintw(ctx->rp_state->inner_win, 8, 2, "Failure records: %i",
              run_sum->nFailureRecords);

    if (run_sum->nTestsFailed > 0)
    {
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(12) | A_BOLD);
        mvwprintw(ctx->rp_state->inner_win, 10, 2, " SOME TESTS FAILED ");
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(12) | A_BOLD);
    }
    else
    {
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(13) | A_BOLD);
        mvwprintw(ctx->rp_state->inner_win, 10, 2, " ALL TESTS PASSED ");
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(13) | A_BOLD);
    }
}

void print_failure_list(APP_CONTEXT *ctx, CU_pFailureRecord fail_rec)
{
    if (fail_rec != NULL)
    {
        wattron(ctx->rp_state->inner_win, A_UNDERLINE);
        mvwprintw(ctx->rp_state->inner_win, 13, 2, "%s", "Failure record");
        wattroff(ctx->rp_state->inner_win, A_UNDERLINE);
    }

    int i = 15;
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
}
