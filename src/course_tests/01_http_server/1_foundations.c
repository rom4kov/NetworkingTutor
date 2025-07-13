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

    if (strcasestr(buf_line->buf_, "Layer 7, Application, HTTP") == NULL)
    {
        return 1;
    }

    // buf_line = buf_line->next;
    // if (strstr("Layer 4", buf_line->buf_) == NULL)
    // {
    //     return 1;
    // }

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
