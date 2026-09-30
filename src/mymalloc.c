#include "mymalloc.h"
#include <stdio.h>
#define MEMLENGTH 4096
#define HEADER_SIZE 8

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;
static int is_initialized = 0;
static char *end_point = heap.bytes + MEMLENGTH;

/** Get the size of current chunk.  */
static unsigned get_cur_chunk_size(char *ptr) { return (ptr[1] + (ptr[2] << 8)); }

/** Get the size of previous chunk. */
static unsigned get_prev_chunk_size(char *ptr) { return (ptr[3] + (ptr[4] << 8)); }

/** Set the size of given ch*/
static void set_chunk_size(char *ptr, size_t size) {
    ptr[0] = size & 0xFF;        // First byte is bottom half of size
    ptr[1] = (size >> 8) & 0xFF; // Second byte is top half
}

/** Traverse through all chunks and see if any are still allocated  */
static void leak_check() {}

/** Set up the first chunk - pointer points to the start of the metadata (8 bytes) */
static void initialize_heap() {
    heap.bytes[0] = 0; // is_allocated = false
    size_t first_chunk_size = MEMLENGTH - HEADER_SIZE;
    // Initialize first chunk size
    set_chunk_size(heap.bytes + 1, first_chunk_size);
    // Run leak check after main exits
    atexit(leak_check);
    is_initialized = 1;
}

/**
 * Allocates memory from the custom heap.
 *
 * @param size Number of bytes to allocate.
 * @param file Source file where allocation was requested.
 * @param line Line number where the allocation was requested.
 * @return Pointer to the allocated memory, or NULL if allocation fails.
 */
void *mymalloc(size_t size, char *file, int line) {
    if (!is_initialized) {
        initialize_heap();
    } // initialize heap if not initialized

    // Go through all the chunks and see if there is one that is big enough and not allocated for the call
    char *chunk_ptr = heap.bytes;
    while (chunk_ptr < end_point) {
        /*
        If current chunk can fill request, change the metadata of the current chunk and
        create a new chunk with leftover space.
        Change current metadata to allocated and correctly change chunk size - create
        new metadata nonallocated with both sizes correct.
        To jump to new chunk, add the header size and chunk size
        */
        unsigned cur_chunk_size = get_cur_chunk_size(chunk_ptr);
        if (chunk_ptr[0] == 0) {
            // If perfect fit, don't split anything
            if (cur_chunk_size == size) {
                // Return a void pointer that points to the payload (metadata pointer + 8 bytes)
                return (void *)chunk_ptr + HEADER_SIZE;
            }
            // If too large, split into two chunks
            else if (cur_chunk_size > size) {
                // Return a void pointer that points to the payload (metadata pointer + 8 bytes)
                return (void *)chunk_ptr + HEADER_SIZE;
            }
        }
        chunk_ptr += (cur_chunk_size + HEADER_SIZE); // move to next chunk
    }
    // Report error if request is too big
    // Be careful to prevent the pointer from going too far
    return NULL;
}

/**
 * @brief Frees allocated memory from the custom heap.
 *
 * @param ptr Pointer to the memory to free.
 * @param file Source file where free was requested.
 * @param line Line number where the free was requested.
 */
void myfree(void *ptr, char *file, int line) {
    if (!is_initialized) {
        initialize_heap();
    }
}