#include "mymalloc.h"
#include <stdio.h>
#include <sys/time.h>

int main() {
    struct timeval start, end;

    gettimeofday(&start, NULL);

    malloc(4);
    malloc(4);
    malloc(4);

    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    printf("Time elapsed: %.6f seconds\n", elapsed);
    return 0;
}
