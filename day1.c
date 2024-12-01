#include <stdio.h>
#include <stdlib.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

int int_cmp(const void *a, const void *b) {
    int ia = *(int *) a;
    int ib = *(int *) b;

    if (ia < ib) return -1;
    if (ib - ia) return  1;
    return 0;
}

typedef struct Frequencies {
    int key;
    int value;
} Frequencies;

int main(int argc, char **argv) {
    int  *left_arr = NULL;
    int *right_arr = NULL;
    int *delta_arr = NULL;

    FILE *input_file = fopen("input/day1.txt", "rb");
    if (input_file) {
        {
            fseek(input_file, 0, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

            char *input = malloc(input_length);
            size_t read = fread(input, sizeof(input[0]), input_length, input_file);

            fclose(input_file);

            for (size_t i = 0; i < input_length; i++) {
                char *end;
                size_t left = strtol(&input[i], &end, 10);
                i += end - &input[i];
                i += 3; //space
                size_t right = strtol(&input[i], &end, 10);
                i += end - &input[i];
                arrput(left_arr, left);
                arrput(right_arr, right);
            }

            free(input);
        }

        qsort(left_arr, arrlenu(left_arr), sizeof(left_arr[0]), int_cmp);
        qsort(right_arr, arrlenu(right_arr), sizeof(right_arr[0]), int_cmp);

        arraddnindex(delta_arr, arrlenu(left_arr));
        for (size_t i = 0; i < arrlenu(delta_arr); i++) {
            delta_arr[i] = abs(left_arr[i] - right_arr[i]);
        }

        size_t result11 = 0;
        for (size_t i = 0; i < arrlenu(delta_arr); i++) {
            result11 += delta_arr[i];
        }

        Frequencies *freqs = NULL;
        hmdefault(freqs, -1);

        for (size_t i = 0; i < arrlenu(left_arr); i++) {
            if (hmget(freqs, left_arr[i]) == -1) {
                size_t freq = 0;
                for (size_t j = 0; j < arrlenu(right_arr); j++) {
                    if (right_arr[j] == left_arr[i]) {
                        freq += 1;
                    }
                }

                hmput(freqs, left_arr[i], freq);
            }
        }

        size_t result12 = 0;
        for (size_t i = 0; i < arrlenu(left_arr); i++) {
            result12 += left_arr[i] * hmget(freqs, left_arr[i]);
        }

        hmfree(freqs);
        arrfree(delta_arr);
        arrfree(left_arr);
        arrfree(right_arr);

        fprintf(stdout, "Day 1-1: %lu\n", result11);
        fprintf(stdout, "Day 1-2: %lu\n", result12);
    }

    return 0;
}
