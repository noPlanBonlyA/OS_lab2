#include "../include/cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    lab2_init(4096, 10);
    int fd = lab2_open("testfile");

    char write_buf[] = "Test data in cache!";
    lab2_write(fd, write_buf, strlen(write_buf));

    char read_buf[50] = {0};
    lab2_lseek(fd, 0, SEEK_SET);
    lab2_read(fd, read_buf, strlen(write_buf));

    printf("Read data: %s\n", read_buf);

    lab2_close(fd);
    return 0;
}
 