CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -fsanitize=address -g -fPIC -D_POSIX_C_SOURCE=200809L
LDFLAGS = -shared

SRC = src/cache.c src/fifo_cache.c
OBJ = $(SRC:.c=.o)
TARGET = libcache.so

.PHONY: all clean

all: $(TARGET) build/test_large build/main build/load_io

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

build/test_large: src/test_large.c $(TARGET)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ src/test_large.c -L. -lcache

build/main: src/main.c $(TARGET)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ src/main.c -L. -lcache

build/load_io: src/load_io.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ src/load_io.c

clean:
	rm -f $(TARGET) build/test_large build/main build/io_load
	rm -rf src/*.o
