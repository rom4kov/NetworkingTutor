#define _POSIX_C_SOURCE 200809L
#include "../../../ntutor.h"
#include "../../data/data_access_layer.h"
#include "../../views/views.h"
#include "../tests.h"
#include "http_server_tests.h"
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pcre2.h>

int server_c_contains_recv_function(char *path)
{
    int rc;

    FILE *server_c_file = fopen(path, "r+");
    if (server_c_file == NULL)
    {
        return 1;
    }

    TEXT_BUFFER *text_buf = initialize_buffer();

    read_file_into_buffer(server_c_file, text_buf);
    LINE *buf_line = text_buf->first_line;

    int pattern_num = 1;

    pcre2_code *re[pattern_num];

    char *patterns[] = {
        "\\brecv\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^,]+,\\s*[^,]+\\)"};

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        bool pattern_matched = false;
        while (buf_line)
        {
            rc =
                check_line_for_matches(buf_line, i, re, strlen(buf_line->buf_));
            if (rc >= 0)
                pattern_matched = true;

            buf_line = buf_line->next;
        }
        if (!pattern_matched)
        {
            fclose(server_c_file);
            return 1;
        }
        buf_line = text_buf->first_line;
    }

    fclose(server_c_file);

    return 0;
}

int server_c_contains_send_function(char *path)
{
    int rc;

    FILE *server_c_file = fopen(path, "r+");
    if (server_c_file == NULL)
    {
        return 1;
    }

    TEXT_BUFFER *text_buf = initialize_buffer();

    read_file_into_buffer(server_c_file, text_buf);
    LINE *buf_line = text_buf->first_line;

    int pattern_num = 1;

    pcre2_code *re[pattern_num];

    char *patterns[] = {
        "\\bsend\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^,]+\\s*,\\s*[^,]+\\s*\\)"};

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        bool pattern_matched = false;
        while (buf_line)
        {
            rc =
                check_line_for_matches(buf_line, i, re, strlen(buf_line->buf_));
            if (rc >= 0)
                pattern_matched = true;

            buf_line = buf_line->next;
        }
        if (!pattern_matched)
        {
            fclose(server_c_file);
            return 1;
        }
        buf_line = text_buf->first_line;
    }

    fclose(server_c_file);

    return 0;
}

int recv_function_works()
{
    FILE *fp = popen("perl -nE 'say $1 if /\\b(\\w+)\\s*=\\s*recv\\s*\\(/' "
                     "http_server/server.c",
                     "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer[32];
    char c;
    int i = 0;
    while (fread(&c, 1, 1, fp))
    {
        buffer[i] = c;
        i++;
    }
    buffer[i - 1] = '\0';
    if (i < 2)
        return 1;

    pclose(fp);

    char perl_command[512];

    snprintf(perl_command, sizeof(perl_command),
             "perl -0777 -pe "
             "'s/(freeaddrinfo\\s*\\(\\s*res\\s*\\)\\s*;)/"
             "printf(\\\"%%i\\\\n\\\", %s);\\n    \\1/' "
             "http_server/server.c > http_server/server_modified2.c",
             buffer);

    FILE *fp1 = popen(perl_command, "r");
    if (fp1 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    pclose(fp1);

    FILE *fp2 = popen("perl -0777 -nE "
                      "'say $1 if /getaddrinfo\\([^,]+,\\s*\"([^,\\n]+)\"/' "
                      "http_server/server.c",
                      "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char port_num[16];
    i = 0;
    while (fread(&c, 1, 1, fp2))
    {
        port_num[i] = c;
        i++;
    }
    port_num[i - 1] = '\0';
    char *port_copy = strdup(port_num);
    int *port_int = (int *)malloc(sizeof(int));
    *port_int = atoi(port_copy);

    pthread_t unblock_server;
    pthread_create(&unblock_server, NULL, connect_to_server, port_int);

    FILE *fp3 = popen(
        "gcc http_server/server_modified2.c -o "
        "http_server/server_modified2 && ./http_server/server_modified2 2>&1",
        "r");
    if (fp3 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    pthread_detach(unblock_server);

    // sleep(1);

    char http_request[512];
    while (fread(&c, 1, 1, fp2))
    {
        http_request[i] = c;
        i++;
    }
    http_request[i - 1] = '\0';
    // fprintf(stderr, "rc: %s\n", rc);
    if (strncmp(http_request, "GET /", 5) == 0)
    {
        pclose(fp3);
        remove("http_server/server_modified2.c");
        remove("http_server/server_modified2");
        return 1;
    }

    pclose(fp3);

    remove("http_server/server_modified2.c");
    remove("http_server/server_modified2");

    return 0;
}

int send_function_works()
{
    bool *res;
    sleep(1);
    FILE *fp = popen("perl -nE 'say $1 if /\\b(\\w+)\\s*=\\s*send\\s*\\(/' "
                     "http_server/server.c",
                     "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer[32];
    char c;
    int i = 0;
    while (fread(&c, 1, 1, fp))
    {
        buffer[i] = c;
        i++;
    }
    buffer[i - 1] = '\0';
    if (i < 2)
        return 1;

    pclose(fp);

    char perl_command[512];

    snprintf(perl_command, sizeof(perl_command),
             "perl -0777 -pe "
             "'s/(freeaddrinfo\\s*\\(\\s*res\\s*\\)\\s*;)/"
             "printf(\\\"%%i\\\\n\\\", %s);\\n    \\1/' "
             "http_server/server.c > http_server/server_modified3.c",
             buffer);

    FILE *fp1 = popen(perl_command, "r");
    if (fp1 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    pclose(fp1);

    FILE *fp2 = popen("perl -0777 -nE "
                      "'say $1 if /getaddrinfo\\([^,]+,\\s*\"([^,\\n]+)\"/' "
                      "http_server/server.c",
                      "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char port_num[16];
    i = 0;
    while (fread(&c, 1, 1, fp2))
    {
        port_num[i] = c;
        i++;
    }
    port_num[i - 1] = '\0';
    char *port_copy = strdup(port_num);
    char perl_command2[512];
    int *port_int = (int *)malloc(sizeof(int));
    *port_int = atoi(port_copy) + 1;

    snprintf(
        perl_command2, sizeof(perl_command2),
        "perl -0777 -pi -e 's/\"%i\"/\"%i\"/' http_server/server_modified3.c",
        *port_int - 1, *port_int);

    system(perl_command2);

    pthread_t unblock_server;
    pthread_create(&unblock_server, NULL, connect_to_server_and_check_response,
                   port_int);

    FILE *fp3 = popen(
        "gcc http_server/server_modified3.c -o "
        "http_server/server_modified3 && ./http_server/server_modified3 2>&1",
        "r");
    if (fp3 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    pthread_join(unblock_server, (void **)&res);

    if (*res == true)
    {
        pclose(fp3);
        remove("http_server/server_modified3.c");
        remove("http_server/server_modified3");
        free(res);
        return 0;
    }

    pclose(fp3);

    remove("http_server/server_modified3.c");
    remove("http_server/server_modified3");
    free(res);

    return 1;
}

void test_if_server_c_contains_recv_function(void)
{
    bool is_socket_call_is_present = false;
    if (server_c_contains_recv_function("http_server/server.c") == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_server_c_contains_send_function(void)
{
    bool is_socket_call_is_present = false;
    if (server_c_contains_send_function("http_server/server.c") == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_recv_works_in_server_c_file(void)
{
    bool is_socket_call_is_present = false;
    if (recv_function_works() == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_send_works_in_server_c_file(void)
{
    bool is_socket_call_is_present = false;
    if (send_function_works() == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void register_section6_tests(APP_CONTEXT *ctx)
{
    ctx->sp[5] = CU_add_suite("http_server_06", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[5], "server.c file exists",
                (CU_TestFunc)test_if_server_c_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[5], "server.c file compiles without errors",
                (CU_TestFunc)test_if_server_c_file_compiles_without_errors);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[5], "server.c contains recv function",
                (CU_TestFunc)test_if_server_c_contains_recv_function);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[5], "server.c contains send function",
                (CU_TestFunc)test_if_server_c_contains_send_function);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    // CU_add_test(ctx->sp[5],
    //             "server.c contains fprintf or perror where necessary",
    //             (CU_TestFunc)test_if_server_c_contains_fprintf_or_perror2);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    // }

    CU_add_test(ctx->sp[5], "recv function in server.c file works",
                (CU_TestFunc)test_if_recv_works_in_server_c_file);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[5], "send function in server.c file works",
                (CU_TestFunc)test_if_send_works_in_server_c_file);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }
    //
    // CU_add_test(ctx->sp[5], "listen error handling in server.c file works",
    //             (CU_TestFunc)test_if_listen_error_handling_works);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    // }
    //
    // CU_add_test(ctx->sp[5], "accept error handling in server.c file works",
    //             (CU_TestFunc)test_if_accept_error_handling_works);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    // }
}
