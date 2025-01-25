#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// Проверка наличия O_DIRECT
#ifndef O_DIRECT
#define O_DIRECT 0
#endif

// Структура блока кэша
typedef struct CacheBlock {
    off_t offset;
    void *data;
    int valid;
} CacheBlock;

// Структура кэша
typedef struct Cache {
    int fd;
    size_t block_size;
    size_t block_count;
    CacheBlock *blocks;
} Cache;

static Cache *cache = NULL;

int lab2_init(const char *path, size_t block_size, size_t block_count) {
    cache = (Cache *)malloc(sizeof(Cache));
    if (!cache) return -1;

    cache->fd = open(path, O_RDWR | O_DIRECT);
    if (cache->fd == -1) {
        perror("Error opening file");
        free(cache);
        return -1;
    }

    posix_fadvise(cache->fd, 0, 0, POSIX_FADV_DONTNEED);

    cache->block_size = block_size;
    cache->block_count = block_count;
    cache->blocks = (CacheBlock *)calloc(block_count, sizeof(CacheBlock));
    if (!cache->blocks) {
        close(cache->fd);
        free(cache);
        return -1;
    }

    for (size_t i = 0; i < block_count; i++) {
        if (posix_memalign(&cache->blocks[i].data, 512, block_size) != 0) {
            perror("Memory allocation failed");
            close(cache->fd);
            free(cache->blocks);
            free(cache);
            return -1;
        }
        cache->blocks[i].valid = 0;
    }

    return 0;
}

int lab2_close() {
    if (!cache) return -1;
    close(cache->fd);
    for (size_t i = 0; i < cache->block_count; i++) {
        free(cache->blocks[i].data);
    }
    free(cache->blocks);
    free(cache);
    cache = NULL;
    return 0;
}
