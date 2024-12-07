#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct Calibration {
    size_t result;
    size_t *operands;
} Calibration;

void increment_ternary(char *ternary) {
    for (size_t i = 0; i < arrlenu(ternary); i += 1) {
        ternary[i] += 1;
        if (ternary[i] < 3) break;

        ternary[i] = 0;
    }
}

void print_ternary(char *ternary) {
    size_t len = arrlenu(ternary);
    for (size_t i = 0; i < len; i += 1) {
        fprintf(stdout, "%d", ternary[len - i - 1]);
    }

    fprintf(stdout, "\n");
}

int main() {
    size_t result1 = 0;
    size_t result2 = 0;

    {
        char *input = NULL;
        FILE *input_file = fopen("input/day7.txt", "rb");
        if (input_file != NULL) {
            fseek(input_file, 0L, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

            input = malloc(input_length);
            int read = fread(input, 1, input_length, input_file);

            fclose(input_file);
        }

        Calibration *calibrations = NULL;

        char *cursor = input;
        while (*cursor != '\0') {
            Calibration *calibration = arraddnptr(calibrations, 1);

            char *end = NULL;
            calibration->result = strtol(cursor, &end, 10);
            cursor = end + 1; // Eat ':'

            calibration->operands = NULL;
            while (*cursor != '\n') {
                cursor += 1; // Eat preceeding space
                arrput(calibration->operands, strtol(cursor, &end, 10));
                cursor = end;
            }

            cursor += 1;
        }

        free(input);

        Calibration *failed_calibrations = NULL;

        for (size_t i = 0; i < arrlenu(calibrations); i += 1) {
            Calibration *c = &calibrations[i];
            int num_operands = arrlenu(c->operands);
            size_t max_operator_mask = 1 << (num_operands - 1);

            // fprintf(stdout, "%lu: ", c->result);

            bool match = false;

            for (size_t operator_mask = 0; operator_mask < max_operator_mask; operator_mask += 1) {
                size_t value = c->operands[0];

                // if (operator_mask > 0) fprintf(stdout, ", ");
                for (size_t operator_index = 0; operator_index < num_operands - 1; operator_index += 1) {
                    size_t operator_id = (operator_mask >> operator_index) & 1;

                    // if (operator_index > 0) fprintf(stdout, " ");
                    // fprintf(stdout, "%c", operator_id ? '*' : '+');

                    size_t operand = c->operands[operator_index + 1];
                    if (operator_id) value *= operand;
                    else             value += operand;
                }

                // fprintf(stdout, " [%lu]", value);

                if (value == c->result) {
                    match = true;
                    break;
                }
            }

            if (match) {
                result1 += c->result;
            } else {
                arrput(failed_calibrations, *c);
            }

            // fprintf(stdout, "\n");
        }

        char *operator_mask_ternary = NULL;
        arrsetlen(operator_mask_ternary, 128);

        char *max_operator_mask_ternary = NULL;
        arrsetlen(max_operator_mask_ternary, 128);

        result2 = result1;

        for (size_t i = 0; i < arrlenu(failed_calibrations); i += 1) {
            Calibration *c = &failed_calibrations[i];
            int num_operands = arrlenu(c->operands);

            memset(operator_mask_ternary, 0, arrlenu(operator_mask_ternary));
            memset(max_operator_mask_ternary, 0, arrlenu(max_operator_mask_ternary));

            max_operator_mask_ternary[num_operands - 1] = 1;

            // fprintf(stdout, "%lu: ", c->result);

            bool match = false;

            bool first = true;
            for (; memcmp(operator_mask_ternary, max_operator_mask_ternary, arrlenu(operator_mask_ternary)) != 0; increment_ternary(operator_mask_ternary)) {
                size_t value = c->operands[0];

                // if (first) fprintf(stdout, ", "); else first = false;

                for (size_t operator_index = 0; operator_index < num_operands - 1; operator_index += 1) {
                    size_t operator_id = operator_mask_ternary[operator_index];

                    // if (operator_index > 0) fprintf(stdout, " ");

                    size_t operand = c->operands[operator_index + 1];

                    switch (operator_id) {
                        case 0: value += operand; /*fprintf(stdout, "+");*/ break;
                        case 1: value *= operand; /*fprintf(stdout, "*");*/ break;
                        case 2: {
                            value = value * (int) powf(10, (int) log10(operand * 10)) + operand;
                            // fprintf(stdout, "|");
                            break;
                        }
                    }
                }

                // fprintf(stdout, " [%lu]", value);

                if (value == c->result) {
                    match = true;
                    break;
                }
            }

            if (match) {
                result2 += c->result;
            }

            // fprintf(stdout, "\n");
        }

        arrfree(failed_calibrations);   //  Next loop will handle operands since these were shallow copies

        for (size_t i = 0; i < arrlenu(calibrations); i += 1) {
            arrfree(calibrations[i].operands);
        }
        arrfree(calibrations);
    }

    fprintf(stdout, "Day 7-1: %lu\n", result1);
    fprintf(stdout, "Day 7-2: %lu\n", result2);

    return 0;
}
