#include "../include/cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define FILE_NAME "testfile"
#define BLOCK_SIZE 4096
#define BLOCK_COUNT 100
#define TEST_SIZE (BLOCK_SIZE * BLOCK_COUNT)

void measure_time(const char *desc, void (*func)(void)) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    func();
    gettimeofday(&end, NULL);
    long elapsed = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
    printf("%s took %ld microseconds.\n", desc, elapsed);
}

void test_read_write() {
    int fd = lab2_open(FILE_NAME);
    if (fd < 0) {
        printf("Error opening file.\n");
        return;
    }

    char *write_buf = malloc(TEST_SIZE);
    if (!write_buf) {
        fprintf(stderr, "Error allocating memory for write buffer.\n");
        exit(EXIT_FAILURE);
    }
    memset(write_buf, 'A', TEST_SIZE);

    printf("Writing large data to file...\n");
    lab2_write(fd, write_buf, TEST_SIZE);
    lab2_fsync(fd);

    char *read_buf = malloc(TEST_SIZE);
    if (!read_buf) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Reading large data from file...\n");
    lab2_lseek(fd, 0, SEEK_SET);
    lab2_read(fd, read_buf, TEST_SIZE);

    if (memcmp(write_buf, read_buf, TEST_SIZE) == 0) {
        printf("Read data matches written data!\n");
    } else {
        printf("Data mismatch!\n");
    }

    free(write_buf);
    free(read_buf);
    lab2_close(fd);
}


  void test_random_access() {
    int fd = lab2_open(FILE_NAME);
    if (fd < 0) {
        fprintf(stderr, "Error opening file.\n");
        return;
    }

    char buf[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_COUNT; i++) {
        off_t offset = (rand() % BLOCK_COUNT) * BLOCK_SIZE;
        
        if (offset < 0 || offset + BLOCK_SIZE > TEST_SIZE) {
            fprintf(stderr, "Error: invalid offset %lld (max: %lld)\n", (long long)offset, (long long)TEST_SIZE);

            lab2_close(fd);
            return;
        }
 
        if (lab2_lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            perror("Error seeking file");
            lab2_close(fd);
            return;
        }

        ssize_t bytes_read = lab2_read(fd, buf, BLOCK_SIZE);
        if (bytes_read != BLOCK_SIZE) {
            fprintf(stderr, "Error reading file at offset %ld, read %zd bytes\n", offset, bytes_read);
            lab2_close(fd);
            return;
        }

        printf("Successfully read block at offset %ld\n", offset);
    }

    printf("Closing file descriptor: %d\n", fd);
    lab2_close(fd);
}


int main() {
    lab2_init(BLOCK_SIZE, BLOCK_COUNT);

    measure_time("Sequential write and read test", test_read_write);
    measure_time("Random access test", test_random_access);

    return 0;
}
