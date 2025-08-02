#include "../../models/models.h"
#include <CUnit/TestRun.h>

// 1_foundations section
void print_run_summary(APP_CONTEXT *ctx, CU_pRunSummary run_sum);
void print_failure_list(APP_CONTEXT *ctx, CU_pFailureRecord fail_rec, int offset);
void test_if_answers_file_exists(void);
void test_if_answers_file_contains_correct_answers(void);
void register_section1_tests(APP_CONTEXT *ctx);

// 2_getaddrinfo section
int server_c_file_exists(char *path);
void test_if_server_c_file_exists(void);
int server_c_file_compiles_without_errors();
void test_if_server_c_file_compiles_without_errors(void);
void register_section2_tests(APP_CONTEXT *ctx);

// 3_socket section
void register_section3_tests(APP_CONTEXT *ctx);
