#include "mymalloc.h"
#include <stdio.h>
#include <sys/time.h>

#define WORKLOAD_RUNS 1

void test_1() {
    unsigned n = 8;
    void *ptr_arr[8];

    for (size_t i = 0; i < 8; i++) {
        ptr_arr[i] = malloc(n);
        n *= 2;
    }

    for (int i = 7; i >= 0; i--) {
        free(ptr_arr[i]);
    }
}

void test_2() {
    unsigned n = 120;
    void *ptr_arr[n];

    for (size_t i = 0; i < n; i++) {
        ptr_arr[i] = malloc(8);
    }

    for (size_t i = 0; i < n; i++) {
        free(ptr_arr[i]);
    }
}

void test_3() {
    unsigned n = 120;
    unsigned total_allocated = 0, cur_allocated = 0;
    void *ptr_arr[n];

    while (total_allocated < n) {
        unsigned to_allocate = rand() % 2;
        if (to_allocate || cur_allocated < 1) {
            ptr_arr[cur_allocated++] = malloc(1);
            total_allocated++;
        } else {
            // Random index
            int idx = rand() % cur_allocated;
            fprintf(stderr, "cur_allocated: %d, idx: %d", cur_allocated, idx);
            // Swap with last object in arr
            void *temp = ptr_arr[cur_allocated - 1];
            ptr_arr[cur_allocated - 1] = ptr_arr[idx];
            ptr_arr[idx] = temp;
            // Free object
            free(ptr_arr[cur_allocated - 1]);
            cur_allocated--;
        }
    }

    for (size_t i = 0; i < cur_allocated; i++) {
        free(ptr_arr[i]);
    }
}

void test_4() {
    // To be implemented...
    return;
}

void test_5() {
    // To be implemented...
    return;
}

int main() {
    struct timeval start, end, cur;
    long seconds = 0, microseconds = 0;
    double total_elapsed = 0.0, avg_elasped = 0.0;

    // Seed RNG with curent time.
    gettimeofday(&cur, NULL);
    srand(cur.tv_usec);

    // Start timer
    gettimeofday(&start, NULL);

    for (size_t i = 0; i < WORKLOAD_RUNS; i++) {
        test_1();
        test_2();
        test_3();
        // test_4();
        // test_5();
    }
    // End timer
    gettimeofday(&end, NULL);

    // Get elapsed time of current runtime.
    seconds = end.tv_sec - start.tv_sec;
    microseconds = end.tv_usec - start.tv_usec;
    // Calculate average time elapsed over each run.
    total_elapsed = seconds + microseconds * 1e-6;
    avg_elasped = total_elapsed / WORKLOAD_RUNS;

    printf("Average time elapsed, %.6f seconds\n", avg_elasped);

    return 0;
}
