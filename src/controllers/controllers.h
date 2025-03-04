#include "../models/models.h"
#include <stdio.h>

void initialize_buffer(TEXT_BUFFER *text_buf, unsigned short num_lines);
LINE *initialize_line();
void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);

