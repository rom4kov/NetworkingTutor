#include "../../models/models.h"
#include <CUnit/TestRun.h>

void print_run_summary(APP_CONTEXT *ctx, CU_pRunSummary run_sum);
void print_failure_list(APP_CONTEXT *ctx, CU_pFailureRecord fail_rec, int offset);
void test_if_answers_file_exists(void);
void test_if_answers_file_contains_correct_answers(void);

