#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct V2 {
    int x, y;
} V2;

typedef struct Antinode_Set {
    V2 key;
    void *value; // unused
} Antinode_Set;

typedef struct {
    V2 *nodes;
} Antenna_Array;

typedef struct Antenna_Map {
    char key;
    Antenna_Array value;
} Antenna_Map;

int main() {
    size_t result1 = 0;
    size_t result2 = 0;

    V2 bounds = { 0 };
    Antenna_Map *map = NULL;
    Antinode_Set *antinodesR1 = NULL;
    Antinode_Set *antinodesR2 = NULL;

    {
        char *input = NULL;
        FILE *input_file = fopen("input/day8.txt", "rb");
        if (input_file != NULL) {
            fseek(input_file, 0L, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

            input = malloc(input_length);
            int read = fread(input, 1, input_length, input_file);

            fclose(input_file);
        }

        char *cursor = input;

        //  Bounds width
        while (*cursor != '\n') {
            bounds.x += 1;
            cursor += 1;
        }
        cursor = input;

        //  Bounds height
        while (*cursor != '\0') {
            if (*cursor == '\n') {
                bounds.y += 1;
            }

            cursor += 1;
        }
        cursor = input;

        //  Parsing grid
        V2 loc = { 0 };
        while (*cursor != '\0') {
            if (*cursor != '.' && *cursor != '\n') {
                Antenna_Map *m = hmgetp_null(map, *cursor);
                if (m == NULL) {
                    hmput(map, *cursor, (Antenna_Array) { 0 });
                    m = hmgetp_null(map, *cursor);
                }

                arrput(m->value.nodes, loc);
            }

            if (*cursor == '\n') {
                loc.x  = 0;
                loc.y += 1;
            } else {
                loc.x += 1;
            }

            cursor += 1;
        }

        free(input);
    }

    for (size_t map_index = 0; map_index < hmlenu(map); map_index += 1) {
        Antenna_Map *m = &map[map_index];

        for (size_t i = 0; i < arrlenu(m->value.nodes); i += 1) {
            V2 loc = m->value.nodes[i];

            if (arrlenu(m->value.nodes) > 1)
            {
                hmput(antinodesR2, loc, NULL);

                for (size_t j = 0; j < arrlenu(m->value.nodes); j += 1) {
                    if (i == j) continue;

                    V2 loc2 = m->value.nodes[j];

                    V2 delta = {
                        loc2.x - loc.x,
                        loc2.y - loc.y,
                    };

                    V2 antinode = {
                        loc.x - delta.x,
                        loc.y - delta.y,
                    };

                    if (antinode.x >= 0 && antinode.y >= 0 && antinode.x < bounds.x && antinode.y < bounds.y) {
                        hmput(antinodesR1, antinode, NULL);
                    }

                    while (antinode.x >= 0 && antinode.y >= 0 && antinode.x < bounds.x && antinode.y < bounds.y) {
                        hmput(antinodesR2, antinode, NULL);
                        antinode.x -= delta.x;
                        antinode.y -= delta.y;
                    }
                }
            }
        }
    }

    result1 = hmlenu(antinodesR1);
    result2 = hmlenu(antinodesR2);

    hmfree(antinodesR1);
    hmfree(antinodesR2);

    for (size_t i = 0; i < hmlenu(map); i += 1) {
        arrfree(map[i].value.nodes);
    }
    hmfree(map);

    fprintf(stdout, "Day 8-1: %lu\n", result1);
    fprintf(stdout, "Day 8-2: %lu\n", result2);

    return 0;
}
