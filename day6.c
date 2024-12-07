#include <stddef.h>
#include <stdio.h>
#include <threads.h>

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

typedef struct V2 {
    int x, y;
} V2;

V2 add_v2(V2 a, V2 b) {
    return (V2) { a.x + b.x, a.y + b.y };
}

typedef enum Facing {
    FN, FE, FS, FW
} Facing;

const V2 Directions[] = {
    [FN] = { 0, -1},
    [FE] = { 1,  0},
    [FS] = { 0,  1},
    [FW] = {-1,  0},
};

const Facing NextFacing[] = {
    [FN] = FE,
    [FE] = FS,
    [FS] = FW,
    [FW] = FN,
};

typedef struct Step {
    V2 loc;
    Facing facing;
} Step;

typedef struct Guard {
    V2 loc;
    Facing facing;
    V2 *history;
    Step *adv_history;
} Guard;

bool contains(V2 n, const V2 *h) {
    for (size_t i = 0; i < arrlenu(h); i += 1) {
        if (h[i].x == n.x && h[i].y == n.y) {
            return true;
        }
    }

    return false;
}

inline
bool is_blocked(V2 loc, const V2 *obstacles, const V2 *additional /*= NULL*/) {
    return contains(loc, obstacles) || (additional && additional->x == loc.x && additional->y == loc.y);
}

inline
bool in_bounds(V2 loc, const V2 bounds) {
    return loc.x >= 0 && loc.x < bounds.x && loc.y >= 0 && loc.y < bounds.y;
}

bool /*looping*/ guard_update(Guard *guard, const V2 *obstacles, const bool record_history, const V2 *additional) {
    if (record_history) {
        if (!contains(guard->loc, guard->history)) {
            arrput(guard->history, guard->loc);
        }
    }

    for (size_t i = 0; i < arrlenu(guard->adv_history); i += 1) {
        Step s = guard->adv_history[i];
        if (s.loc.x == guard->loc.x && s.loc.y == guard->loc.y && s.facing == guard->facing) {
            return true;
        }
    }

    arrput(guard->adv_history, ((Step) { guard->loc, guard->facing }));

    V2 next_space = add_v2(guard->loc, Directions[guard->facing]);

    while (is_blocked(next_space, obstacles, additional)) {
        Facing new_facing = NextFacing[guard->facing];
        guard->facing = new_facing;

        next_space = add_v2(guard->loc, Directions[guard->facing]);
    }

    guard->loc = next_space;

    return false;
}

typedef struct Job {
    size_t id;

    Guard guard;
    V2 *obstacles;
    V2 new_obstacle;
    V2 bounds;
} Job;

typedef struct Batch {
    size_t id;

    Job *jobs;
} Batch;

int execute_batch(void *batch_pointer) {
    Batch *batch = (Batch *) batch_pointer;

    int result = 0;

    for (size_t i = 0; i < arrlenu(batch->jobs); i += 1) {
        Job *job = &batch->jobs[i];
        while (in_bounds(job->guard.loc, job->bounds)) {
            bool looping = guard_update(&job->guard, job->obstacles, false, &job->new_obstacle);
            if (looping) {
                result += 1;
                break;
            }
        }

        // fprintf(stdout, "Job %lu-%lu complete\n", batch->id, job->id);
        // thrd_yield();
    }

    return result;
}

int main() {
    size_t result1 = 0;
    size_t result2 = 0;

    V2 guard_start = { 0 };
    V2 bounds = { 0 };
    V2 *obstacles = NULL;
    Guard guard = { 0 };

    {
        char *input = NULL;
        FILE *input_file = fopen("input/day6.txt", "rb");
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
            switch (*cursor) {
                case  '#': arrput(obstacles, loc); break;
                case  '^': guard_start = loc; guard.loc = loc; break;
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

    while (in_bounds(guard.loc, bounds)) {
        guard_update(&guard, obstacles, true, NULL);
    }
    result1 = arrlenu(guard.history);

    Batch *batches = NULL;
    int jobs_per_batch = 8;
    int batch_index = 0;

    arrput(batches, ((Batch) { .id = batch_index }));

    //  Build batches
    for (size_t y = 0; y < bounds.y; y += 1) {
        for (size_t x = 0; x < bounds.x; x += 1) {
            V2 new = {x, y};
            if (!contains(new, guard.history)) continue;

            if (!contains(new, obstacles) && !(new.x == guard_start.x && new.y == guard_start.y)) {
                Job job = { 0 };

                job.id = arrlenu(batches[batch_index].jobs);
                job.guard = (Guard) { .loc = guard_start };
                job.bounds = bounds;
                job.obstacles = obstacles;
                job.new_obstacle = new;

                if (arrlenu(batches[batch_index].jobs) == jobs_per_batch) {
                    batch_index += 1;
                    arrput(batches, ((Batch) { .id = batch_index }));
                    arrsetcap(batches[batch_index].jobs, jobs_per_batch);
                }

                arrput(batches[batch_index].jobs, job);
            }
        }
    }

    //  Execute jobs
    #define NUM_THREADS 512

    thrd_t threads[NUM_THREADS] = { 0 };

    int runs = arrlenu(batches) / NUM_THREADS;
    int last_run_count = arrlenu(batches) % NUM_THREADS;
    for (size_t i = 0; i < runs; i += 1 ) {
        for (size_t j = 0; j < NUM_THREADS; j += 1 ) {
            thrd_create(&threads[j], execute_batch, &batches[i * NUM_THREADS + j]);
        }

        for (size_t j = 0; j < NUM_THREADS; j += 1 ) {
            int result = 0;
            thrd_join(threads[j], &result);

            result2 += result;
        }
    }

    for (size_t i = 0; i < last_run_count; i += 1 ) {
        thrd_create(&threads[i], execute_batch, &batches[runs * NUM_THREADS + i]);
    }

    for (size_t i = 0; i < last_run_count; i += 1 ) {
        int result = 0;
        thrd_join(threads[i], &result);

        result2 += result;
    }

    //  Clean-up jobs
    for (size_t i = 0; i < arrlenu(batches); i += 1) {
        for (size_t j = 0; j < arrlenu(batches[i].jobs); j += 1) {
            arrfree(batches[i].jobs[j].guard.history);
            arrfree(batches[i].jobs[j].guard.adv_history);
            // arrfree(batches[i].jobs[j].obstacles);
        }

        arrfree(batches[i].jobs);
    }

    arrfree(batches);

    fprintf(stdout, "Day 6-1: %lu\n", result1);
    fprintf(stdout, "Day 6-2: %lu\n", result2);

    arrfree(guard.history);
    arrfree(guard.adv_history);
    arrfree(obstacles);

    return 0;
}
