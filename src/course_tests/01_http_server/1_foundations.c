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
    LINE *buf_line = text_buf->first_line;

    read_file_into_buffer(answers_file, text_buf);

    if (strstr("Layer 7", buf_line->buf_) == NULL)
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
    CU_ASSERT(answers_file_exists("http_server/answers.txt") == 0);
}

void test_if_answers_file_contains_correct_answers(void)
{
    CU_ASSERT(answers_file_contains_correct_answers("http_server/answers.txt") == 0);
}
