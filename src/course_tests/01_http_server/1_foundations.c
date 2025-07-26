#define _GNU_SOURCE
#include "../../data/data_access_layer.h"
#include <CUnit/CUnit.h>
#include <stdio.h>

int answers_file_exists(char *path)
{
    FILE *answers_file = fopen(path, "r+");
    if (answers_file == NULL)
    {
        return 1;
    }
    fclose(answers_file);

    return 0;
}

int answers_file_contains_correct_answers(char *path)
{
    FILE *answers_file = fopen(path, "r+");
    if (answers_file == NULL)
    {
        return 1;
    }

    TEXT_BUFFER *text_buf = initialize_buffer();

    read_file_into_buffer(answers_file, text_buf);
    LINE *buf_line = text_buf->first_line;

    if (buf_line->next == NULL)
    {
        return 1;
    }

    if (strcasestr(buf_line->buf_, "Layer 7, Application, HTTP") == NULL ||
        strcasestr(buf_line->next->buf_, "Layer 4, Transport, TCP") == NULL ||
        strcasestr(buf_line->next->next->buf_, "Layer 3, Network, IP") == NULL)
    {
        return 1;
    }

    fclose(answers_file);

    return 0;
}

void test_if_answers_file_exists(void)
{
    bool does_answers_file_exist = false;
    if (answers_file_exists("http_server/answers.txt") == 0)
    {
        does_answers_file_exist = true;
    }
    CU_ASSERT(does_answers_file_exist);
}

void test_if_answers_file_contains_correct_answers(void)
{
    bool does_answers_file_contain_correct_answers = false;
    if (answers_file_contains_correct_answers("http_server/answers.txt") == 0)
    {
        does_answers_file_contain_correct_answers = true;
    }
    CU_ASSERT(does_answers_file_contain_correct_answers);
}

void register_section1_tests(APP_CONTEXT *ctx)
{
    ctx->sp[0] = CU_add_suite("http_server_01", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[0], "answers file exists",
                (CU_TestFunc)test_if_answers_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[0], "answers file contains correct answers",
                (CU_TestFunc)test_if_answers_file_contains_correct_answers);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }
}
