#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct File_Block {
    int64_t file_id;
    int64_t block_index;
} File_Block;

int compare_file_blocks(const void *fba, const void *fbb) {
    const File_Block *a = fba;
    const File_Block *b = fbb;

    return a->block_index > b->block_index;
}

int main() {
    size_t result1 = 0;
    size_t result2 = 0;

    int64_t *free_list1 = NULL;
    int64_t *free_list2 = NULL;
    File_Block *file_blocks1 = NULL;
    File_Block *file_blocks2 = NULL;

    {
        char *input = NULL;
        FILE *input_file = fopen("input/day9.txt", "rb");
        if (input_file != NULL) {
            fseek(input_file, 0L, SEEK_END);
            size_t input_length = ftell(input_file);
            rewind(input_file);

            input = malloc(input_length);
            int read = fread(input, 1, input_length, input_file);

            fclose(input_file);
        }

        char *cursor = input;

        size_t block_index = 0;
        while (*cursor != '\0' && *cursor != '\n') {
            size_t cursor_index = cursor - input;
            size_t run_length   = *cursor - '0';

            if (cursor_index % 2 == 0) {
                int64_t file_id = cursor_index / 2;
                for (size_t i = 0; i < run_length; i += 1) {
                    arrput(file_blocks1, ((File_Block) { file_id, block_index }));
                    arrput(file_blocks2, ((File_Block) { file_id, block_index }));
                    block_index += 1;
                }
            } else {
                for (size_t i = 0; i < run_length; i += 1) {
                    arrput(free_list1, block_index);
                    arrput(free_list2, block_index);
                    block_index += 1;
                }
            }

            cursor += 1;
        }

        free(input);
    }

    {
        size_t free_list_index = 0;
        for (int64_t i = arrlenu(file_blocks1) - 1; i >= 0; i -= 1) {
            File_Block *block = &file_blocks1[i];
            size_t free_block_index = free_list1[free_list_index];
            if (free_block_index < block->block_index) {
                arrput(free_list1, block->block_index);
                block->block_index = free_block_index;
                free_list_index += 1;
            }
        }

        qsort(file_blocks1, arrlenu(file_blocks1), sizeof(File_Block), compare_file_blocks);
//
//         for (int i = 0; i < arrlenu(file_blocks1); i += 1) {
//             File_Block *block = &file_blocks[i];
//             fprintf(stdout, "{ %lu, %lu }\n", block->block_index, block->file_id);
//         }
//         fprintf(stdout, "\n");

        for (int i = 0; i < arrlenu(file_blocks1); i += 1) {
            File_Block *block = &file_blocks1[i];
            result1 += block->file_id * block->block_index;
        }
    }

    {
        size_t free_list_index = 0;
        for (int64_t i = arrlenu(file_blocks2) - 1; i >= 0;) {
            File_Block *block = &file_blocks2[i];

            size_t file_size = 0;
            for (int64_t j = i; j >= 0; j -= 1) {
                if (file_blocks2[j].file_id != block->file_id) {
                    break;
                }

                file_size += 1;
            }

            // fprintf(stdout, "id: %lu, size: %lu\n", block->file_id, file_size);

            int64_t free_index = 0;
            int64_t free_block_index = free_list2[0];
            int64_t free_size  = 1;
            for (int64_t j = 1; j < arrlenu(free_list2); j += 1) {
                if (free_size >= file_size) break;

                int64_t delta = free_list2[j] - free_list2[j - 1];
                if (delta > 1) {
                    free_index = j;
                    free_block_index = free_list2[j];
                    free_size = 1;
                } else {
                    free_size += 1;
                }
            }

            if (free_block_index < block->block_index) {
                if (free_size >= file_size) {
                    arrdeln(free_list2, free_index, free_size);

                    for (int64_t j = 0; j < file_size; j += 1) {
                        int64_t fbi = i - file_size + 1 + j;
                        arrput(free_list2, file_blocks2[fbi].block_index);
                        file_blocks2[fbi].block_index = free_block_index;

                        free_block_index += 1;
                    }
                }
            }

            i -= file_size;
        }

        qsort(file_blocks2, arrlenu(file_blocks2), sizeof(File_Block), compare_file_blocks);
// 
//         for (int i = 0; i < arrlenu(file_blocks2); i += 1) {
//             File_Block *block = &file_blocks2[i];
//             fprintf(stdout, "%lu", block->file_id);
//         }
//         fprintf(stdout, "\n");

        for (int i = 0; i < arrlenu(file_blocks2); i += 1) {
            File_Block *block = &file_blocks2[i];
            result2 += block->file_id * block->block_index;
        }
    }

    arrfree(file_blocks1);
    arrfree(file_blocks2);
    arrfree(free_list1);
    arrfree(free_list2);

    fprintf(stdout, "Day 9-1: %lu\n", result1);
    fprintf(stdout, "Day 9-2: %lu\n", result2);

    return 0;
}
