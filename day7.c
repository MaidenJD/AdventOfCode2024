#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct Calibration {
    size_t result;
    size_t *operands;
} Calibration;

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

        for (size_t i = 0; i < arrlenu(calibrations); i += 1) {
            Calibration *c = &calibrations[i];
            int num_operands = arrlenu(c->operands);
            size_t max_operator_mask = 1 << (num_operands - 1);

            // fprintf(stdout, "%lu: ", c->result);

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
                    result1 += value;
                    break;
                }
            }

            // fprintf(stdout, "\n");
        }

        for (size_t i = 0; i < arrlenu(calibrations); i += 1) {
            arrfree(calibrations[i].operands);
        }
        arrfree(calibrations);
    }

    fprintf(stdout, "Day 7-1: %lu\n", result1);
    fprintf(stdout, "Day 7-2: %lu\n", result2);

    return 0;
}
