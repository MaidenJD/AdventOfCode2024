#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> //debug

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct Rule {
    size_t former;
    size_t latter;
} Rule;

int find(size_t *arr, size_t n) {
    for (size_t i = 0; i < arrlenu(arr); i += 1) {
        if (arr[i] == n) return i;
    }

    return -1;
}

Rule *rules = NULL;

void arrprint(size_t *arr) {
    for (size_t i = 0; i < arrlenu(arr); i += 1) {
        if (i == 0) {
            fprintf(stdout, "%lu", arr[i]);
        } else {
            fprintf(stdout, " %lu", arr[i]);
        }
    }

    fprintf(stdout, "\n");
}

bool pages_pass(Rule *rules, size_t *pages) {
    for (size_t i = 0; i < arrlenu(rules); i += 1) {
        int fi = find(pages, rules[i].former);
        int li = find(pages, rules[i].latter);

        if (fi != -1 && li != -1) {
            if (fi > li) {
                return false;
            }
        }
    }

    return true;
}

Rule find_rule(Rule *rules, size_t a, size_t b) {
    for (size_t i = 0; i < arrlenu(rules); i += 1) {
        Rule rule = rules[i];
        if ((rule.former == a || rule.former == b) && (rule.latter == a || rule.latter == b)) {
            return rule;
        }
    }

    return (Rule) {-1, -1};
}

int compare_pages(const void *lhs, const void *rhs) {
    size_t a = *(size_t *)lhs;
    size_t b = *(size_t *)rhs;

    Rule rule = find_rule(rules, a, b);
    if (a == rule.former) {
        return -1;
    } else {
        return 1;
    }
}

int main(int argc, char **argv) {
    size_t result1 = 0;
    size_t result2 = 0;

    FILE *input_file = fopen("input/day5.txt", "rb");
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

        size_t *pages = NULL;

        char *cursor = input;
        while (*cursor != '\0') {
            if (*(cursor + 2) != '|') break;

            Rule *rule = arraddnptr(rules, 1);

            char *end = NULL;
            rule->former = strtol(cursor, &end, 10);
            cursor = end + 1; // +1 to eat |
            rule->latter = strtol(cursor, &end, 10);
            cursor = end + 1; // +1 to eat \n
        }

        cursor += 1; // Eat empty line

        while (*cursor != '\0') {
            while (*cursor != '\n') {
                char *end = NULL;
                int n = strtol(cursor, &end, 10);
                cursor = end;

                arrput(pages, n);

                if (*cursor == ',') {
                    cursor += 1; // Eat ,
                }
            }

            if (pages_pass(rules, pages)) {
                result1 += pages[(arrlenu(pages) + 1) / 2 - 1];
            } else {
                qsort(pages, arrlenu(pages), sizeof(size_t), compare_pages);
                result2 += pages[(arrlenu(pages) + 1) / 2 - 1];
            }

            arrfree(pages);

            cursor += 1;
        }

        arrfree(rules);

        fprintf(stdout, "Day 5-1: %lu\n", result1);
        fprintf(stdout, "Day 5-2: %lu\n", result2);

        free(input);
    }
}

