#include "mymalloc.h"
#include <stdio.h>
#define MEMLENGTH 4096
#define HEADER_SIZE 8

#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...) ((void)0)
#endif

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;
static int is_initialized = 0;
static char *end_point = heap.bytes + MEMLENGTH;

/** Get the size of current chunk.  */
static size_t get_cur_chunk_size(char *ptr) {
    return ((unsigned char *)ptr)[1] + (((unsigned char *)ptr)[2] << 8);
}

/** Get the size of previous chunk. */
static size_t get_prev_chunk_size(char *ptr) {
    return ((unsigned char *)ptr)[3] + (((unsigned char *)ptr)[4] << 8);
}

/** Set the size of current chunk. */
static void set_cur_chunk_size(char *ptr, size_t size) {
    ptr[1] = size & 0xFF;        // First byte is bottom half of size
    ptr[2] = (size >> 8) & 0xFF; // Second byte is top half
}

/** Set the size of previous chunk. */
static void set_prev_chunk_size(char *ptr, size_t size) {
    ptr[3] = size & 0xFF;        // First byte is bottom half of size
    ptr[4] = (size >> 8) & 0xFF; // Second byte is top half
}

/** Traverse through all chunks and see if any are still allocated  */
static void leak_check() {
    DEBUG_PRINT("leak_check called.\n");
    char *chunk_ptr = heap.bytes;
    unsigned allocated_chunks = 0;
    size_t allocated_bytes = 0;

    while (chunk_ptr < end_point) {
        size_t cur_chunk_size = get_cur_chunk_size(chunk_ptr);
        DEBUG_PRINT("cur: %p, size = %zu\n", chunk_ptr, cur_chunk_size);
        if (*chunk_ptr) {
            allocated_chunks++;
            allocated_bytes += cur_chunk_size;
        }

        chunk_ptr += (cur_chunk_size + HEADER_SIZE);
    }

    if (allocated_chunks > 0) {
        fprintf(stderr, "myalloc: %zu bytes leaked in %d objects.\n", allocated_bytes,
                allocated_chunks);
    }
}

/** Set up the first chunk - pointer points to the start of the metadata (8 bytes) */
static void initialize_heap() {
    DEBUG_PRINT("Initializing heap...\n");
    heap.bytes[0] = 0; // is_allocated = false
    size_t first_chunk_size = MEMLENGTH - HEADER_SIZE;

    set_cur_chunk_size(heap.bytes, first_chunk_size); // Initialize first chunk size
    atexit(leak_check);                               // Run leak check after main exits
    is_initialized = 1;                               // Update flag

    DEBUG_PRINT("Heap initalization sucessful.\n");
}

/**
 * @brief Allocates memory from the custom heap.
 *
 * @param size Number of bytes to allocate.
 * @param file Source file where allocation was requested.
 * @param line Line number where the allocation was requested.
 * @return Pointer to the allocated memory, or NULL if allocation fails.
 */
void *mymalloc(size_t size, char *file, int line) {
    size = (size + 7) & ~7; // Round to nearest multiple of 8
    DEBUG_PRINT("malloc called with size: %zu\n", size);
    // Initialize heap if not initialized
    if (!is_initialized) {
        initialize_heap();
    }

    // Go through all the chunks and see if there is one that is big enough and not allocated
    char *chunk_ptr = heap.bytes;
    while (chunk_ptr < end_point) {
        /*
        If current chunk can fill request, change the metadata of the current chunk and
        create a new chunk with leftover space.
        Change current metadata to allocated and correctly change chunk size - create
        new metadata nonallocated with both sizes correct.
        To jump to new chunk, add the header size and chunk size
        */
        size_t cur_chunk_size = get_cur_chunk_size(chunk_ptr);
        DEBUG_PRINT("cur: %p, size = %zu\n", chunk_ptr, cur_chunk_size);

        if (chunk_ptr[0] == 0 && cur_chunk_size >= size) {
            chunk_ptr[0] = 1;                    // Set chunk header to "allocated".
            set_cur_chunk_size(chunk_ptr, size); // Update chunk size in header.

            // If too large, split into two chunks
            size_t remaining_size = cur_chunk_size - size - HEADER_SIZE;
            DEBUG_PRINT("Cur size; %zu\n", cur_chunk_size);
            if (remaining_size > 0) {
                char *next_chunk_ptr = chunk_ptr + HEADER_SIZE + size;
                DEBUG_PRINT("Cur: %p, New: %p.\n", chunk_ptr, next_chunk_ptr);
                // Initialize the new chunk.
                next_chunk_ptr[0] = 0;
                set_cur_chunk_size(next_chunk_ptr, remaining_size);
                set_prev_chunk_size(next_chunk_ptr, size);
                DEBUG_PRINT("Split chunk: %zu\n", remaining_size);
            }

            // Return a void pointer that points to the payload (metadata pointer + 8 bytes)
            return (void *)chunk_ptr + HEADER_SIZE;
        }
        chunk_ptr += (cur_chunk_size + HEADER_SIZE); // move to next chunk
    }
    // Report error if request is too big (careful for pointer out of bounds)
    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s.c:%d)", size, file, line);
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
    DEBUG_PRINT("free called with ptr: %p\n", ptr);
    if (!is_initialized) {
        initialize_heap();
    }
}