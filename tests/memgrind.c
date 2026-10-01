#include "mymalloc.h"
#include <stdio.h>
#include <sys/time.h>

#define WORKLOAD_RUNS 50

void test_1() {
    unsigned n = 8;
    void *ptr_arr[8];

    for (size_t i = 0; i < 8; i++) {
        ptr_arr[i] = malloc(n);
        n *= 2;
    }

    for (size_t i = 7; i > 0; i--) {
        free(ptr_arr[i]);
        n /= 2;
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
    unsigned allocated_cnt = 0;
    unsigned to_allocate = 0;
    void *ptr_arr[n];

    while (allocated_cnt < 120) {
        to_allocate = rand() % 2;
        if (to_allocate) {
            ptr_arr[allocated_cnt] = malloc(1);
        } else {
            int idx = rand() % allocated_cnt;
            free(ptr_arr[idx]);
            allocated_cnt--;
        }
    }
    for (size_t i = 0; i < n; i++) {
        free(ptr_arr[i]);
    }
}

int main() {
    struct timeval start, end, cur;
    long seconds = 0, microseconds = 0;
    double elapsed_ms = 0.0, total_elapsed = 0.0, avg_elasped = 0.0;

    // Seed RNG with curent time.
    srand(gettimeofday(&cur, NULL));

    for (size_t i = 0; i < WORKLOAD_RUNS; i++) {
        gettimeofday(&start, NULL);

        test_1();
        test_2();

        gettimeofday(&end, NULL);

        // Calculate elapsed time of current runtime
        seconds = end.tv_sec - start.tv_sec;
        microseconds = end.tv_usec - start.tv_usec;
        elapsed_ms = seconds + microseconds * 1e-6;

        printf("Time elapsed: %.6f seconds\n", elapsed_ms);
        total_elapsed += elapsed_ms;
    }

    avg_elasped = total_elapsed / WORKLOAD_RUNS;
    printf("Average time elapsed, %.6f seconds\n", avg_elasped);

    return 0;
}
