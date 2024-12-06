#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Offset {
    int x, y;
} Offset;

const Offset offsets1[] = {
    {-1,  0},
    {-1, -1},
    { 0, -1},
    { 1, -1},
    { 1,  0},
    { 1,  1},
    { 0,  1},
    {-1,  1},
};

int main(int argc, char **argv) {
    size_t result1 = 0;
    size_t result2 = 0;

    FILE *input_file = fopen("input/day4.txt", "rb");
    if (input_file) {
        char *input = NULL;

        {
            fseek(input_file, 0, SEEK_END);
            size_t input_length = ftell(input_file);
            if (input_length == 0) {
                fclose(input_file);
                return 1;
            }

            rewind(input_file);

            input = malloc(input_length);
            size_t read = fread(input, sizeof(input[0]), input_length, input_file);

            fclose(input_file);
        }

        char *cursor = input;
        int width = 0;
        while (*cursor != '\n') {
            width += 1;
            cursor += 1;
        }

        cursor = input;
        int height = 0;
        while (*cursor != '\0') {
            if (*cursor == '\n') {
                height += 1;
            }

            cursor += 1;
        }

        #define COORD_TO_INDEX(X, Y) (Y) * (width + 1) + (X) /* +1 to include \n characters */

        for (int y = 0; y < height; y += 1) {
            for (int x = 0; x < width; x += 1) {
                char c = input[COORD_TO_INDEX(x, y)];

                if (c == 'X') {
                    for (int i = 0; i < 8; i += 1) {
                        int x1 = x;
                        int y1 = y;

                        Offset offset = offsets1[i];

                        x1 += offset.x;
                        y1 += offset.y;
                        if (x1 < 0 || x1 >= width)  continue;
                        if (y1 < 0 || y1 >= height) continue;

                        c = input[COORD_TO_INDEX(x1, y1)];
                        if (c != 'M') continue;

                        x1 += offset.x;
                        y1 += offset.y;
                        if (x1 < 0 || x1 >= width)  continue;
                        if (y1 < 0 || y1 >= height) continue;

                        c = input[COORD_TO_INDEX(x1, y1)];
                        if (c != 'A') continue;

                        x1 += offset.x;
                        y1 += offset.y;
                        if (x1 < 0 || x1 >= width)  continue;
                        if (y1 < 0 || y1 >= height) continue;

                        c = input[COORD_TO_INDEX(x1, y1)];
                        if (c != 'S') continue;

                        result1 += 1;
                    }
                } else if (c == 'A') {
                    if (y > 0 && y < height - 1 && x > 0 && x < width - 1) {
                        char ul = input[COORD_TO_INDEX(x - 1, y - 1)];
                        char ur = input[COORD_TO_INDEX(x + 1, y - 1)];
                        char ll = input[COORD_TO_INDEX(x - 1, y + 1)];
                        char lr = input[COORD_TO_INDEX(x + 1, y + 1)];

                        bool d1 = (ul == 'M' && lr == 'S') || (ul == 'S' && lr == 'M');
                        bool d2 = (ur == 'M' && ll == 'S') || (ur == 'S' && ll == 'M');

                        if (d1 && d2) {
                            result2 += 1;
                        }
                    }
                }
            }
        }

        free(input);
    }

    fprintf(stdout, "Day 4-1: %lu\n", result1);
    fprintf(stdout, "Day 4-2: %lu\n", result2);

    return 0;
}
