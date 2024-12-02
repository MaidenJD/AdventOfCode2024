#include <stdio.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

int main(int argc, char **argv) {
    FILE *input_file = fopen("input/day2.txt", "rb");
    if (input_file) {
        int **reports = NULL;

        {
            fseek(input_file, 0, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

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

        size_t result11 = 0;
        for (size_t i = 0; i < arrlenu(reports); i++) {
            int *report = reports[i];

            if (arrlenu(report) >= 2) {
                int expected_direction = report[1] - report[0];
                expected_direction = expected_direction / abs(expected_direction);
                if (expected_direction == 0) continue; // Unsafe report, duplicate levels!

                bool safe = true;
                for (size_t j = 1; j < arrlenu(report); j++) {
                    int delta = report[j] - report[j - 1];
                    if (delta == 0) {
                        //  Delta is 0, unsafe due to duplicate levels
                        safe = false;
                        break;
                    }

                    int size = abs(delta);
                    if (size > 3) {
                        //  Delta is too large
                        safe = false;
                        break;
                    }

                    int direction = delta / size;
                    if (direction != expected_direction) {
                        //  Direction has changed
                        safe = false;
                        break;
                    }
                }

                if (safe) result11 += 1;
            } else {
                // Single item report, automatically safe!
                result11 += 1;
            }
        }

        for (size_t i = 0; i < arrlenu(reports); i++) {
            arrfree(reports[i]);
        }

        arrfree(reports);

        fprintf(stdout, "Day 2-1: %lu\n", result11);
        // fprintf(stdout, "Day 2-2: %lu\n", result12);
    }

    return 0;
}
