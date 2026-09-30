CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined -Iinclude -DDEBUG

all: build/memtest build/memgrind

build:
	mkdir -p build

build/mymalloc.o: src/mymalloc.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c src/mymalloc.c -o build/mymalloc.o

build/memtest.o: tests/memtest.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c tests/memtest.c -o build/memtest.o

build/memgrind.o: tests/memgrind.c include/mymalloc.h | build
	$(CC) $(CFLAGS) -c tests/memgrind.c -o build/memgrind.o

build/memtest: build/memtest.o build/mymalloc.o
	$(CC) $(CFLAGS) build/memtest.o build/mymalloc.o -o build/memtest

build/memgrind: build/memgrind.o build/mymalloc.o
	$(CC) $(CFLAGS) build/memgrind.o build/mymalloc.o -o build/memgrind

test: build/memtest
	./build/memtest

run: build/memgrind
	./build/memgrind

clean:
	rm -rf build

.PHONY: all test run clean