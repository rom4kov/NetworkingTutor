#define PCRE2_CODE_UNIT_WIDTH 8
#include "../models/models.h"
#include "pcre2.h"

int check_line_for_matches(LINE *current_line, int j, pcre2_code **re,
                      int subject_length)
{
    PCRE2_SPTR subject = (PCRE2_SPTR)current_line->buf_;
    pcre2_match_data *md = pcre2_match_data_create_from_pattern(re[j], NULL);
    if (!md)
        return -1;

    return pcre2_match(re[j], subject, subject_length, 0, 0, md, NULL);
}
