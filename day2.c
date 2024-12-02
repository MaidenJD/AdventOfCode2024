#include <stdio.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

bool is_report_safe(int *report, size_t report_length) {
    if (report_length >= 2) {
        int expected_direction = report[1] - report[0];
        expected_direction = expected_direction / abs(expected_direction);
        if (expected_direction == 0) return false; // Unsafe report, duplicate levels!

        for (size_t j = 1; j < report_length; j++) {
            int delta = report[j] - report[j - 1];
            if (delta == 0) {
                //  Delta is 0, unsafe due to duplicate levels
                return false;
            }

            int size = abs(delta);
            if (size > 3) {
                //  Delta is too large
                return false;
            }

            int direction = delta / size;
            if (direction != expected_direction) {
                //  Direction has changed
                return false;
            }
        }
    }

    return true;
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("input/day2.txt", "rb");
    if (input_file) {
        int **reports = NULL;

        {
            fseek(input_file, 0, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

            if (input_length == 0) {
                fprintf(stderr, "Failed to read input\n");
                return 1;
            }

            char *input = malloc(input_length);
            size_t read = fread(input, sizeof(input[0]), input_length, input_file);

            fclose(input_file);

            char *end;
            char *cursor = input;

            int *report = NULL;
            while (*cursor != '\0') {
                size_t val = strtol(cursor, &end, 10);
                // fprintf(stdout, "%lu", val);
                arrput(report, val);

                cursor = end;

                if (*cursor == '\n') {
                    // fprintf(stdout, "\n");
                    arrput(reports, report);
                    report = NULL;
                } else {
                    // fprintf(stdout,  " ");
                }

                cursor += 1;
            }

            free(input);
        }

        int scratch_buffer[1024] = { 0 };

        size_t result1 = 0;
        size_t result2 = 0;
        for (size_t report_index = 0; report_index < arrlenu(reports); report_index++) {
            int *report = reports[report_index];

            bool safe_at_first_glance = is_report_safe(report, arrlenu(report));
            if (safe_at_first_glance) {
                result1 += 1;
                result2 += 1;
            } else {
                for (size_t skip_index = 0; skip_index < arrlenu(report); skip_index++) {
                    size_t scratch_index = 0;
                    for (size_t i = 0; i < skip_index; i++, scratch_index++) {
                        scratch_buffer[scratch_index] = report[i];
                    }

                    for (size_t i = skip_index + 1; i < arrlenu(report); i++, scratch_index++) {
                        scratch_buffer[scratch_index] = report[i];
                    }

                    if (is_report_safe(scratch_buffer, arrlenu(report) - 1)) {
                        result2 += 1;
                        break;
                    }
                }
            }
        }

        for (size_t i = 0; i < arrlenu(reports); i++) {
            arrfree(reports[i]);
        }

        arrfree(reports);

        fprintf(stdout, "Day 2-1: %lu\n", result1);
        fprintf(stdout, "Day 2-2: %lu\n", result2);
    }

    return 0;
}
