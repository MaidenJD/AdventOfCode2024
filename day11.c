#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct Entry_Count {
    size_t key;
    size_t value;
} Entry_Count;

void iterate(size_t iterations, Entry_Count **front, Entry_Count **back) {
    for (size_t iteration = 0; iteration < iterations; iteration += 1) {
        for (size_t i = 0; i < hmlenu(*front); i += 1) {
            Entry_Count entry = (*front)[i];
            size_t val        = entry.key;
            size_t occurances = entry.value;

            if (val == 0) {
                size_t count = occurances;
                if (hmgeti(*back, 1) >= 0) {
                    count += hmget(*back, 1);
                }

                hmput(*back, 1, count);
            } else {
                size_t width = (int) log10(val * 10);
                if (width % 2 == 0) {
                    size_t mask = (int) pow(10, width / 2);
                    size_t left = val / mask;
                    size_t right = val - left * mask;

                    {
                        size_t count = occurances;
                        if (hmgeti(*back, left) >= 0) {
                            count += hmget(*back, left);
                        }

                        hmput(*back, left, count);
                    }

                    {
                        size_t count = occurances;
                        if (hmgeti(*back, right) >= 0) {
                            count += hmget(*back, right);
                        }

                        hmput(*back, right, count);
                    }
                } else {
                    size_t mul = val * 2024;

                    size_t count = occurances;
                    if (hmgeti(*back, mul) >= 0) {
                        count += hmget(*back, mul);
                    }

                    hmput(*back, mul, count);
                }
            }
        }

        {
            Entry_Count *temp = *front;
            *front = *back;
            *back = temp;

            hmfree(*back);
            *back = NULL;
            hmdefault(*back, 0);
        }
    }
}

int main() {
    size_t result1 = 0;
    size_t result2 = 0;

    Entry_Count *front = NULL;
    hmdefault(front, 0);

    Entry_Count *back  = NULL;
    hmdefault(back, 0);

    {
        char *input = NULL;
        FILE *input_file = fopen("input/day11.txt", "rb");
        if (input_file != NULL) {
            fseek(input_file, 0L, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

            input = arraddnptr(input, input_length);
            int read = fread(input, 1, input_length, input_file);

            fclose(input_file);
        }

        char *cursor = input;

        while (*cursor != '\0') {
            char *end = NULL;
            size_t val = strtol(cursor, &end, 10);
            cursor = end;

            hmput(front, val, 1);

            cursor += 1; // For whitespace
        }

        arrfree(input);
    }

    iterate(25, &front, &back);

    for (size_t i = 0; i < hmlenu(front); i += 1) {
        result1 += front[i].value;
    }

    //  Cummulative with the previous loop, totals 75 iterations
    iterate(50, &front, &back);

    for (size_t i = 0; i < hmlenu(front); i += 1) {
        result2 += front[i].value;
    }

    hmfree(front);
    hmfree(back);

    fprintf(stdout, "Day 11-1: %lu\n", result1);
    fprintf(stdout, "Day 11-2: %lu\n", result2);

    return 0;
}
