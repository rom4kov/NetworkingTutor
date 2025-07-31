#define _POSIX_C_SOURCE 2
#include "../../data/data_access_layer.h"
#include "../../views/views.h"
#include "../tests.h"
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

int server_c_file_exists(char *path)
{
    FILE *server_file = fopen(path, "r+");
    if (server_file == NULL)
    {
        return 1;
    }
    fclose(server_file);

    return 0;
}

void test_if_server_c_file_exists(void)
{
    bool does_server_file_exist = false;
    if (server_c_file_exists("http_server/server.c") == 0)
    {
        does_server_file_exist = true;
    }
    CU_ASSERT(does_server_file_exist);
}

void register_section2_tests(APP_CONTEXT *ctx)
{
    ctx->sp[1] = CU_add_suite("http_server_02", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[1], "server.c file exists",
                (CU_TestFunc)test_if_server_c_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }
}
