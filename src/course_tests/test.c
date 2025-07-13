#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include "01_http_server/http_server_tests.h"

#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <ncurses.h>
#include <unistd.h>

int perform_tests(APP_CONTEXT *ctx)
{
    bool is_in_failure_list = false;

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
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(sp, "answers file exists",
                (CU_TestFunc)test_if_answers_file_exists);
    ec = CU_get_error();
    if (ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(sp, "answers file contains correct answers",
                (CU_TestFunc)test_if_answers_file_contains_correct_answers);
    ec = CU_get_error();
    if (ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_basic_set_mode(CU_BRM_SILENT);

    int saved_stdout = dup(STDOUT_FILENO);
    suppress_stdout();

    ec = CU_basic_run_tests();

    restore_stdout(saved_stdout);

    if (ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_pRunSummary run_sum = CU_get_run_summary();
    print_run_summary(ctx, run_sum);

    int offset = 0;
    int num_of_successes = CU_get_number_of_successes();
    if (num_of_successes > 0)
        offset = num_of_successes;

    wattron(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->rp_state->inner_win, 13, 0, "%s", "Details");
    wattroff(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);

    CU_pFailureRecord fail_rec = CU_get_failure_list();

    print_failure_list(ctx, fail_rec, offset);

    int i = 0;
    CU_pTestRegistry tr = CU_get_registry();
    CU_pSuite suite = CU_get_suite_by_name("http_server_test_suite_01", tr);
    CU_pTest test = suite->pTest;

    if (run_sum->nTestsFailed == 0)
    {
        ctx->rp_state->sections_completed++;
        ctx->rp_state->test_mode = false;
        while (test != NULL)
        {
            wattron(ctx->rp_state->inner_win, COLOR_PAIR(4));
            mvwprintw(ctx->rp_state->inner_win, 15 + i, 0, "%s PASSED",
                      test->pName);
            wattroff(ctx->rp_state->inner_win, COLOR_PAIR(4));
            test = test->pNext;
            i++;
        }
    }
    else
    {
        while (test)
        {
            CU_pFailureRecord first_rec = fail_rec;
            while (fail_rec)
            {
                if (strcmp(test->pName, fail_rec->pTest->pName) == 0)
                {
                    is_in_failure_list = true;
                }
                fail_rec = fail_rec->pNext;
            }
            if (is_in_failure_list == false)
            {
                wattron(ctx->rp_state->inner_win, COLOR_PAIR(4));
                mvwprintw(ctx->rp_state->inner_win, 15 + i, 0, "%s PASSED",
                          test->pName);
                wattroff(ctx->rp_state->inner_win, COLOR_PAIR(4));
                i++;
            }
            is_in_failure_list = false;
            test = test->pNext;
            fail_rec = first_rec;
        }
    }

    wrefresh(ctx->rp_state->inner_win);
    print_section_or_task_compl_msg(ctx, run_sum);
    // wnoutrefresh(ctx->course_windows[4]);
    doupdate();

    CU_cleanup_registry();
    return 0;
}

void print_run_summary(APP_CONTEXT *ctx, CU_pRunSummary run_sum)
{
    wattron(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->rp_state->inner_win, 0, 0, "%s", "TEST RESULTS");
    wattroff(ctx->rp_state->inner_win, A_UNDERLINE | A_BOLD);
    mvwprintw(ctx->rp_state->inner_win, 2, 0, "Suites run: %i",
              run_sum->nSuitesRun);
    mvwprintw(ctx->rp_state->inner_win, 3, 0, "Suites failed: %i",
              run_sum->nSuitesFailed);
    mvwprintw(ctx->rp_state->inner_win, 4, 0, "Tests run: %i",
              run_sum->nTestsRun);

    // print how many tests failed, colored
    if (run_sum->nTestsFailed > 0)
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(4));
    mvwprintw(ctx->rp_state->inner_win, 5, 0, "Tests failed: %i",
              run_sum->nTestsFailed);
    if (run_sum->nTestsFailed > 0)
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(4));

    mvwprintw(ctx->rp_state->inner_win, 6, 0, "Asserts: %i", run_sum->nAsserts);

    // print how many asserts failed, colored
    if (run_sum->nAssertsFailed > 0)
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(4));
    mvwprintw(ctx->rp_state->inner_win, 7, 0, "Asserts failed: %i",
              run_sum->nAssertsFailed);
    if (run_sum->nAssertsFailed > 0)
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(3));
    else
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(4));

    mvwprintw(ctx->rp_state->inner_win, 8, 0, "Failure records: %i",
              run_sum->nFailureRecords);

    if (run_sum->nTestsFailed > 0)
    {
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(12) | A_BOLD);
        mvwprintw(ctx->rp_state->inner_win, 10, 0, " SOME TESTS FAILED ");
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(12) | A_BOLD);
    }
    else
    {
        wattron(ctx->rp_state->inner_win, COLOR_PAIR(13) | A_BOLD);
        mvwprintw(ctx->rp_state->inner_win, 10, 0, " ALL TESTS PASSED ");
        wattroff(ctx->rp_state->inner_win, COLOR_PAIR(13) | A_BOLD);
    }
}

void print_failure_list(APP_CONTEXT *ctx, CU_pFailureRecord fail_rec,
                        int offset)
{
    int i = 15 + offset;

    wattron(ctx->rp_state->inner_win, COLOR_PAIR(3));
    while (fail_rec != NULL)
    {
        mvwprintw(ctx->rp_state->inner_win, i, 0, "%s FAILED",
                  fail_rec->pTest->pName);
        mvwprintw(ctx->rp_state->inner_win, i + 1, 0, "Line number: %i",
                  fail_rec->uiLineNumber);
        mvwprintw(ctx->rp_state->inner_win, i + 2, 0, "Filename: %s",
                  fail_rec->strFileName);
        mvwprintw(ctx->rp_state->inner_win, i + 3, 0, "Condition: %s",
                  fail_rec->strCondition);
        mvwprintw(ctx->rp_state->inner_win, i + 4, 0, "Suite name: %s",
                  fail_rec->pSuite->pName);
        fail_rec = fail_rec->pNext;
        i += 6;
    }
    wattroff(ctx->rp_state->inner_win, COLOR_PAIR(3));
}
