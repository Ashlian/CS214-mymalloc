#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

void test_1() {
    char *p = malloc(5000);
    free(p);
}

void test_2() {
    char x = 20;
    char *p = &x;
    free(p);
}

void test_3() {
    char *p = malloc(160);
    free(p + 1);
}

void test_4() {
    char *p = malloc(160);
    free(p);
    free(p);
}

void test_5() {
    char *a = malloc(10);
    char *b = malloc(20);
}

int main(int argc, char **argv){
    if (argc < 2) {
        fprintf(stderr, "You must input a number from 1-5 into %s\n", argv[0]);
        return 1;
    }
    int test_num = atoi(argv[1]);
    switch (test_num) {
        // Test 1: Free checks for null pointers (invalid)
        case 1:
            test_1();
            break;
        // Test 2: Free checks for pointers out of heap bounds (not from malloc)
        case 2:
            test_2();
            break;
        // Test 3: Free checks for pointers in the middle of chunks
        case 3:
            test_3();
            break;
        // Test 4: Free accounts for double free calls
        case 4:
            test_4();
            break;
        // Test 5: Memory leaks are caught and logged
        case 5:
            test_5();
            break;
        default:
            printf("Not a valid test number (1-5)\n");
            break;
    }
    return 0;
}