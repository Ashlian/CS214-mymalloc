#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if (argc < 2){
        fprintf(stderr, "You must input a number from 1-5 into %s\n", argv[0]);
        return 1;
    }
    int test_num = atoi(argv[1]);
    if (test_num < 1 || test_num > 5){
        printf("Not a valid test number (1-5)\n");
    }
    // Free checks for null pointers

    // Free checks for pointers out of heap bounds (not from malloc)

    // Free checks for pointers in the middle of chunks

    // Free accounts for double free calls

    // Memory leaks are caught and logged
    return 0;
}