#include "mymalloc.h"
#include <stdio.h>
#include <stdint.h>

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

/** 
 *
 */
void test_1() {
    char *x = malloc(4000);
    char *y = malloc(4000);
    if (y == NULL) {
        printf("Test 1: Malloc successfully returns NULL\n");
    } 
    else {
        printf("Test 1: Malloc failed to return NULL\n");
        free(y);
    }
    free(x);
}

/** 
 *
 */
void test_2() {
    for(int i = 0; i < 50; i++) {
        char *x = malloc(4000);
        if(x == NULL) {
            printf("Test 2: Free fails to make memory reusable\n");
            return;
        }
        free(x);
        char *y = malloc(4000);
        if(y == NULL) {
            printf("Test 2: Free fails to make memory reusable\n");
            return;
        }
        free(y);
    }
    printf("Test 2: Free successfully deallocates memory\n");
}

/** 
 *
 */
void test_3() {
    size_t sizes[] = {1,3,6,9,12,21,33,41,48,50};
    #define NSIZES (sizeof(sizes) / sizeof(sizes[0]))
    char *ptrs[NSIZES];
    int i, j, alignment_errors = 0;

    // Allocate different sizes and check if divisible by 8
    for (i = 0; i < NSIZES; i++) {
        ptrs[i] = malloc(sizes[i]);
        if(ptrs[i] == NULL) {
            printf("Unable to allocate object %d\n", i);
            exit(EXIT_FAILURE);
        }
        else if ((uintptr_t)ptrs[i] % 8 != 0) {
            alignment_errors++;
        }
    }
    printf("Test 3: %d incorrect alignments\n", alignment_errors);

    // Free all the pointers
    for (j = 0; j < NSIZES; j++) {
        free(ptrs[j]);
    }
}

/** 
 *
 */
void test_4() {
    char *obj[OBJECTS];
    int i, j;

    // fill memory with objects
    for (i = 0; i < OBJECTS; i++) {
        obj[i] = malloc(OBJSIZE);
        if (obj[i] == NULL) {
            printf("Unable to allocate object %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Merge with prev chunk
    free(obj[0]);
    free(obj[1]);
    char *a = malloc(2 * OBJSIZE);
    if (a == NULL) {
        printf("Test 4: Free fails to merge with previous chunk\n");
        return;
    }
    free(a);

    // Merge with next chunk
    free(obj[4]);
    free(obj[3]);
    char *b = malloc(2 * OBJSIZE);
    if (b == NULL) {
        printf("Test 4: Free fails to merge with the next chunk\n");
        return;
    }
    free(b);
    
    // Merge with both sides
    free(obj[6]);
    free(obj[8]);
    free(obj[7]);
    char *c = malloc(3 * OBJSIZE);
    if (c == NULL) {
        printf("Test 4: Free fails to merge with both sides\n");
        return;
    }
    free(c);
    
    // Free the rest of the pointers
    free(obj[2]);
    free(obj[5]);
    for(j = 9; j < OBJECTS; j++) {
        free(obj[j]);
    }
    printf("Test 4: Free successfully coalesces chunks\n");

}

/** 
 *
 */
void test_5() {

}

int main(int argc, char **argv){
    // Test 1: Malloc return NULL on failure
    test_1();

    // Test 2: Free makes memory reusable (actually deallocates memory)
    test_2();

    // Test 3: Malloc successfully aligns memory by 8
    test_3();

    // Test 4: Free coalesces adjacent free chunks correctly
    test_4();

    // Test 5: Allocated chunks never resize unless explicitly requested
    test_5();

    return 0;
}