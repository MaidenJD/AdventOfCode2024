#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    size_t result1 = 0;
    size_t result2 = 0;

    FILE *input_file = fopen("input/day3.txt", "rb");
    if (input_file) {
        {
            fseek(input_file, 0, SEEK_END);
            size_t input_length = ftell(input_file);
            if (input_length == 0) {
                fclose(input_file);
                return 1;
            }

            rewind(input_file);

            char *input = malloc(input_length);
            size_t read = fread(input, sizeof(input[0]), input_length, input_file);

            fclose(input_file);

            bool mul_enabled = true;

            char *cursor = input;
            while (*cursor != '\0') {
                if (memcmp(cursor, "mul", 3) == 0) {
                    cursor += 3;

                    if (*cursor == '(') {
                        cursor += 1;

                        char *end;
                        size_t lhs = strtol(cursor, &end, 10);

                        if (cursor != end) {
                            cursor = end;

                            if (*cursor == ',') {
                                cursor += 1;

                                char *end;
                                size_t rhs = strtol(cursor, &end, 10);

                                if (cursor != end) {
                                    cursor = end;

                                    if (*cursor == ')') {
                                        cursor += 1;

                                        size_t mul = lhs * rhs;
                                        result1 += mul;

                                        if (mul_enabled) {
                                            result2 += mul;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else if (memcmp(cursor, "do()", 4) == 0) {
                    cursor += 4;

                    mul_enabled = true;
                } else if (memcmp(cursor, "don't()", 7) == 0) {
                    cursor += 7;

                    mul_enabled = false;
                } else {
                    cursor += 1;
                }
            }

            free(input);
        }

        fprintf(stdout, "Day 3-1: %lu\n", result1);
        fprintf(stdout, "Day 3-2: %lu\n", result2);
    }

    return 0;
}
