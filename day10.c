#include <stddef.h>
#include <stdio.h>
#include <threads.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct V2 {
    int x, y;
} V2;

typedef struct Cell {
    V2 loc;
    char height;
} Cell;

typedef struct Peak_Set {
    int64_t key;
    void *value;    // unused
} Peak_Set;

typedef struct Trail {
    V2 key;           // trailhead
    Peak_Set *value;  // num_trails
} Trail;

typedef struct Unique_Trail {
    V2 key;
    int64_t value;
} Unique_Trail;

int64_t get_index_for_cell_location(const V2 loc, const V2 bounds) {
    if (loc.x < 0 || loc.y < 0 || loc.x >= bounds.x || loc.y >= bounds.y) return -1;

    return loc.y * bounds.x + loc.x;
}

int main() {
    size_t result1 = 0;
    size_t result2 = 0;

    V2 bounds = { 0 };
    Cell *heightmap = NULL;

    int64_t *peak_indices = NULL;
    Trail *trail_heads = NULL;
    hmdefault(trail_heads, NULL);

    Unique_Trail *unique_trails = NULL;
    hmdefault(unique_trails, 0);

    {
        char *input = NULL;
        FILE *input_file = fopen("input/day10.txt", "rb");
        if (input_file != NULL) {
            fseek(input_file, 0L, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

            input = arraddnptr(input, input_length);
            int read = fread(input, 1, arrlenu(input), input_file);

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
        while (cursor - input < arrlenu(input)) {
            if (*cursor >= '0' && *cursor <= '9') {
                arrput(heightmap, ((Cell) { loc, *cursor - '0' }));

                if (*cursor == '9') {
                    arrput(peak_indices, arrlenu(heightmap) - 1);
                }
            }

            if (*cursor == '\n') {
                loc.x  = 0;
                loc.y += 1;
            } else {
                loc.x += 1;
            }

            cursor += 1;
        }

        int64_t num_entries = arrlenu(heightmap);
        arrfree(input);
    }

    Cell *trail = NULL;
    for (size_t i = 0; i < arrlenu(peak_indices); i += 1) {
        int64_t peak_index = peak_indices[i];
        arrput(trail, heightmap[peak_index]);

        while (arrlenu(trail) > 0) {
            Cell cell = arrpop(trail);

            if (cell.height == 0) {
                Peak_Set *set = hmget(trail_heads, cell.loc);
                hmput(set, peak_index, NULL);
                hmput(trail_heads, cell.loc, set);

                int64_t val = hmget(unique_trails, cell.loc);
                hmput(unique_trails, cell.loc, val + 1);
            } else {
                int64_t indices[4] = {
                    get_index_for_cell_location((V2) { cell.loc.x,     cell.loc.y - 1 }, bounds),
                    get_index_for_cell_location((V2) { cell.loc.x,     cell.loc.y + 1 }, bounds),
                    get_index_for_cell_location((V2) { cell.loc.x - 1, cell.loc.y     }, bounds),
                    get_index_for_cell_location((V2) { cell.loc.x + 1, cell.loc.y     }, bounds),
                };

                for (size_t j = 0; j < 4; j += 1) {
                    if (indices[j] != -1) {
                        Cell next_cell = heightmap[indices[j]];

                        bool valid = next_cell.height < cell.height && abs(next_cell.height - cell.height) == 1;

                        if (valid) {
                            arrput(trail, next_cell);
                        }
                    }
                }
            }
        }
    }

    arrfree(trail);

    for (size_t i = 0; i < hmlenu(trail_heads); i += 1) {
        result1 += hmlenu(trail_heads[i].value);
    }

    for (size_t i = 0; i < hmlenu(unique_trails); i += 1) {
        result2 += unique_trails[i].value;
    }

    hmfree(unique_trails);
    hmfree(trail_heads);
    arrfree(heightmap);
    arrfree(peak_indices);

    fprintf(stdout, "Day 10-1: %lu\n", result1);
    fprintf(stdout, "Day 10-2: %lu\n", result2);

    return 0;
}
