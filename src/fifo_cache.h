#ifndef FIFO_CACHE_H
#define FIFO_CACHE_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct CacheBlock {
    off_t offset;
    void *data;
    int valid;
} CacheBlock;

typedef struct FIFO_Cache {
    CacheBlock *blocks;
    size_t block_size;
    size_t block_count;
    size_t head;
    size_t tail;
} FIFO_Cache;
 
FIFO_Cache* fifo_cache_init(size_t block_size, size_t block_count);
void fifo_cache_store(FIFO_Cache *cache, off_t offset, const void *data);
CacheBlock* fifo_cache_lookup(FIFO_Cache *cache, off_t offset);
void fifo_cache_free(FIFO_Cache *cache);

#endif
