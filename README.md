# CS214-Mymalloc
Project 1 of CS214 Systems Programming

By: Jeffrey Lu (jl3925) & Alexander Wong (amw408)

## Run Instructions

### Building
From the project root, run `make` to build every program. The executables are placed in the `build/` directory. To remove all build output, run `make clean`.

### Running
- `make test` runs `memtest`, the basic allocation test.
- `make run` runs `memgrind`, the performance stress test.
- `make correct` runs `memcorrect`, which runs all of the correctness tests and prints the result of each.
- `make errtest-N` runs `memerr` with the input N, which selects one error or leak test (N is 1 through 5, described below).

The programs can also be run directly from the `build/` directory, for example `./build/memcorrect` or `./build/memerr 3`.

## Test Plan

Our test plan consists of two programs memcorrect.c and memerr.c. memcorrect.c takes no inputs and runs tests that checks whether or not the features of malloc and free function correctly. memerr.c takes one input that decides the test being run and contains tests that ensure malloc and free catch errors that should terminate the function or program. The following tests are included in our test programs:

**Test: Malloc catches a request that is too large (memcorrect.c, runs automatically)**
- **Requirement:** When given a request that no chunk can fulfill, malloc returns a null pointer.
- **Method:** Request 4000 bytes from malloc and store it in pointer x. Then request another 4000 bytes and store in the pointer y. Check if y is a null pointer, then free both x and y to prevent leaks.
- **Expected Result:** y is a null pointer and malloc should print that it cannot allocate the 4000 bytes. Leak checker also reports nothing at exit.
- **Catches:** Malloc that does not check size constraints, which could override other chunks or go out of bounds.

**Test: Free makes memory reusable (memcorrect.c, runs automatically)**
- **Requirement:** When a chunk is freed, the memory it takes up can be used by another malloc call.
- **Method:** Request 4000 bytes from malloc and store it in pointer x. Check if x is NULL. If it is, then print test fail and return. If it is not, then free x and request another 4000 bytes from malloc and store it in y. Check if y is NULL. If it is, then print test fail and return. If it is not, then free y. Repeat this 50 times.
- **Expected Result:** Memory should be recycled between the malloc and free calls, allowing the test to run to completion with no errors. Leak checker also reports nothing at exit.
- **Catches:** Free that loses bytes over time or locks out other malloc calls.

**Test: Malloc successfully aligns to 8 (memcorrect.c, runs automatically)**
- **Requirement:** All pointers given by malloc are divisible by 8.
- **Method:** Define an array of character pointers and define a set of sizes not divisible by 8. For each size, allocate this many bytes using malloc and store it in one of the pointers in the array. After allocating the memory, check that the pointer address is divisible by 8 and increase the count of errors if it is not. Print the count of alignment errors and free all the pointers at the end.
- **Expected Result:** Every pointer should have an address divisible by 8, so there should be 0 alignment errors. Leak checker also reports nothing at exit.
- **Catches:** Malloc that does not align pointers correctly.

**Test: Free coalesces chunks correctly (memcorrect.c, runs automatically)**
- **Requirement:** When two adjacent chunks are freed, they become one chunk, allowing for a larger malloc request to fit.
- **Method:** Fill the heap with 64 objects of size OBJSIZE. Free a chunk and then the chunk after it to force a left coalesce. Call malloc with 2 * OBJSIZE and check if it is NULL. Free a different chunk and then the chunk before it to force a right coalesce. Call malloc with 2 * OBJSIZE and check if it is NULL. Free two chunks surrounding another chunk and then the chunk itself to force a left and right coalesce. Call malloc with 3 * OBJSIZE and check if it is NULL. Finally, free all of the remaining chunks.
- **Expected Result:** The malloc calls only work if the unallocated chunks merge together correctly. Hence, all of the malloc calls should not return NULL, and the test should run to completion without any errors. Leak checker also reports nothing at exit.
- **Catches:** Coalescing that does not change sizes or forgets to check for both the previous or next chunk.

**Test: Live chunks never resize (memcorrect.c, runs automatically)**
- **Requirement:** Allocated chunks keep their size and contents through any later malloc/free calls.
- **Method:** Fill the heap with 64 objects, each with its own byte pattern. Free all but every third object so each survivor has free neighbors on both sides. Next, request an object too large for any two-chunk gap, then refill the gaps with two-chunk objects and their own patterns. Finally, free all the remaining objects.
- **Expected Result:** The oversized request returns NULL, every survivor and new object still holds its pattern, and the leak checker reports nothing at exit.
- **Catches:** Coalescing that absorbs an allocated neighbor, chunk splits that contain wrong boundaries, size errors that misalign traversal.

**Test: Freeing invalid pointers (memerr.c, runs when inputting 1)**
- **Requirement:** Free checks for null pointers and returns if one is requested.
- **Method:** Request 5000 bytes from malloc and store the pointer which should be NULL. Then, free this null pointer.
- **Expected Result:** Free does not do anything and the program runs to completion.
- **Catches:** Free trying to access and dereference null pointers.

**Test: Freeing pointers not from malloc (memerr.c, runs when inputting 2)**
- **Requirement:** Free checks for pointers outside the heap bounds and prints an error message if one is requested.
- **Method:** Initialize a character in memory with some value and define a pointer to the address of the character. Then, free this pointer
- **Expected Result:** Free prints that the pointer is out of bounds and terminates the program with exit code 2.
- **Catches:** Free trying to deallocate chunks that are not a part of the heap.

**Test: Freeing pointers in the middle of chunks (memerr.c, runs when inputting 3)**
- **Requirement:** Free checks that pointers point to the beginning of chunks and prints an error message if one is requested.
- **Method:** Request 160 bytes from malloc and store the pointer. Then, call free using the address 1 byte ahead of the pointer.
- **Expected Result:** Free prints that the pointer is not at the start of the chunk and terminates the program with exit code 2.
- **Catches:** Free trying to deallocate chunks using pointers not from the beginning of the chunk, which ruins the deallocation.

**Test: Double free calls (memerr.c, runs when inputting 4)**
- **Requirement:** Free checks for pointers that are already freed and prints an error message if one is requested.
- **Method:** Request 160 bytes from malloc and store the pointer. Then, call free on the pointer twice.
- **Expected Result:** Free prints that a double free was called and terminates the program with exit code 2.
- **Catches:** Free trying to deallocate a chunk that is already freed, which is impossible.

**Test: Memory leaks (memerr.c, runs when inputting 5)**
- **Requirement:** Memory leaks are checked and logged with the number of bytes and objects leaked.
- **Method:** Request 10 bytes from malloc and then another 20 bytes from malloc and do not free any of them.
- **Expected Result:** The leak checker should print that there were 40 bytes leaked in 2 objects.
- **Catches:** Memory leaks that are unaccounted for or calculated wrong.