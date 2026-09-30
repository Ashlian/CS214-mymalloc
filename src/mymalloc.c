#include "mymalloc.h"
#define MEMLENGTH 4096
#define HEADER_SIZE 8

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;
static int is_initialized = 0;
static char *end_point = heap.bytes + MEMLENGTH;

// Takes in the pointer to the chunk size data and returns the actual size
static unsigned get_chunk_size(char *p) { return (p[0] + (p[1] << 8)); }

static void set_chunk_size(char *p, size_t size) {
    p[0] = size & 0xFF;        // First byte is bottom half of size
    p[1] = (size >> 8) & 0xFF; // Second byte is top half
}

// Leak check
static void leak_check() {
    // Traverse through all chunks and see if any are still allocated
}

// Sets up the first chunk - pointer points to the start of the metadata (8
// bytes)
static void initialize_heap() {
    heap.bytes[0] = 0; // is_allocated = false
    size_t first_chunk_size = MEMLENGTH - HEADER_SIZE;
    set_chunk_size(heap.bytes + 1,
                   first_chunk_size); // Initialize first chunk size
    atexit(leak_check);               // Run leak check after main exits
    is_initialized = 1;
}

void *mymalloc(size_t size, char *file, int line) {
    if (!is_initialized) {
        initialize_heap();
    } // initialize heap if not initialized

    // Go through all the chunks and see if there is one that is big enough and
    // not allocated for the call
    char *chunk_pointer = heap.bytes;
    while (chunk_pointer < end_point) {
        /*
        If current chunk can fill request, change the metadata of the current
        chunk and create a new chunk with leftover space Change current metadata
        to allocated and correctly change chunk size - create new metadata
        nonallocated with both sizes correct To jump to new chunk, add the
        header size and chunk size
        */
        unsigned chunk_size = get_chunk_size(chunk_pointer + 1);
        if ((chunk_pointer[0] == 0)) {
            // if perfect fit, don't split anything
            if (chunk_size == size) {

                return (void *)chunk_pointer +
                       HEADER_SIZE; // Return a void pointer that points to the
                                    // payload (metadata pointer + 8 bytes)
            }
            // split the chunk into two chunks
            else if (chunk_size > size) {

                return (void *)chunk_pointer +
                       HEADER_SIZE; // Return a void pointer that points to the
                                    // payload (metadata pointer + 8 bytes)
            }
        }
        chunk_pointer += (chunk_size + HEADER_SIZE); // move to next chunk
    }
    // Report error if request is too big - Be careful to prevent the pointer
    // from going too far
    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    if (!is_initialized) {
        initialize_heap();
    } // initialize heap if not initialized
}