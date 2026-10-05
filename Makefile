CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined -Iinclude 

all: build/memtest build/memgrind build/memcorrect build/memerr

build:
	mkdir -p build

build/mymalloc.o: src/mymalloc.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c src/mymalloc.c -o build/mymalloc.o

build/memtest.o: tests/memtest.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c tests/memtest.c -o build/memtest.o

build/memgrind.o: tests/memgrind.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c tests/memgrind.c -o build/memgrind.o

build/memcorrect.o: tests/memcorrect.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c tests/memcorrect.c -o build/memcorrect.o

build/memerr.o: tests/memerr.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c tests/memerr.c -o build/memerr.o

build/memtest: build/memtest.o build/mymalloc.o
	$(CC) $(CFLAGS) build/memtest.o build/mymalloc.o -o build/memtest

build/memgrind: build/memgrind.o build/mymalloc.o
	$(CC) $(CFLAGS) build/memgrind.o build/mymalloc.o -o build/memgrind

build/memcorrect: build/memcorrect.o build/mymalloc.o
	$(CC) $(CFLAGS) build/memcorrect.o build/mymalloc.o -o build/memcorrect

build/memerr: build/memerr.o build/mymalloc.o
	$(CC) $(CFLAGS) build/memerr.o build/mymalloc.o -o build/memerr

test: build/memtest  
	./build/memtest

run: build/memgrind
	./build/memgrind

correct: build/memcorrect
	./build/memcorrect

errtest-%: build/memerr
	./build/memerr $*

clean:
	rm -rf build

.PHONY: all test run correct clean