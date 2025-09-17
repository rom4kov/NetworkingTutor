#include "../../ntutor.h"
#include "../models/models.h"

#include <pcre2.h>

int initialize_testing(APP_CONTEXT *ctx);
int perform_tests(APP_CONTEXT *ctx);
int check_line_for_matches(LINE *current_line, int j, pcre2_code **re,
                      int subject_length);
void *connect_to_server(void *arg);
void *connect_to_server_and_check_response(void *arg);

