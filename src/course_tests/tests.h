#include "../models/models.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

int perform_tests(APP_CONTEXT *ctx);
int check_line_for_matches(LINE *current_line, int j, pcre2_code **re,
                      int subject_length);

