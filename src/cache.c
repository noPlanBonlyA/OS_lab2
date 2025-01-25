#include "../include/cache.h"
#include "fifo_cache.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>  
 

static int file_fd = -1;
static FIFO_Cache *cache = NULL;

int lab2_open(const char *path) {
    file_fd = open(path, O_RDWR | O_CREAT, 0666);
    if (file_fd == -1) {
        perror("Error opening file");
        return -1;
    }

#ifdef __APPLE__
    if (fcntl(file_fd, F_NOCACHE, 1) == -1) {
        perror("Error setting F_NOCACHE");
        close(file_fd);
        return -1;
    }
#endif

#ifdef __linux__
    #ifdef _POSIX_C_SOURCE
        if (posix_fadvise(file_fd, 0, 0, POSIX_FADV_DONTNEED) != 0) {
            perror("Error setting posix_fadvise");
            close(file_fd);
            return -1;
        }
    #else
        printf("Warning: posix_fadvise is not available.\n");
    #endif
#endif

    return file_fd;
}

int lab2_close(int fd) {
    if (file_fd == -1) {
        fprintf(stderr, "Error: File already closed or not opened.\n");
        return -1;
    }

    close(fd);

    if (cache) {
        fifo_cache_free(cache);
        cache = NULL;  
          }

    file_fd = -1;
    return 0;
}


ssize_t lab2_read(int fd, void *buf, size_t count) {
#ifdef __APPLE__
    posix_fadvise(fd, 0, count, POSIX_FADV_DONTNEED);
#endif
    return read(fd, buf, count);
}

ssize_t lab2_write(int fd, const void *buf, size_t count) {
#ifdef __APPLE__
    posix_fadvise(fd, 0, count, POSIX_FADV_DONTNEED);
#endif
    return write(fd, buf, count);
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}

int lab2_fsync(int fd) {
    return fsync(fd);
}

int lab2_init(size_t block_size, size_t block_count) {
    cache = fifo_cache_init(block_size, block_count);
    return cache ? 0 : -1;
}
