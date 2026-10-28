CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -g -Iinclude
LIB     = src/heap.c src/hashtable.c src/graph.c src/linkedlist.c

all: build/emergency

build:
	mkdir -p build

build/emergency: build src/main.c $(LIB)
	$(CC) $(CFLAGS) src/main.c $(LIB) -o $@

build/test_dsa: build tests/test_dsa.c $(LIB)
	$(CC) $(CFLAGS) tests/test_dsa.c $(LIB) -o $@

run: build/emergency
	./build/emergency

test: build/test_dsa
	./build/test_dsa

clean:
	rm -rf build
