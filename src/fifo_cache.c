#include "fifo_cache.h"
#include <string.h>

FIFO_Cache* fifo_cache_init(size_t block_size, size_t block_count) {
    FIFO_Cache *cache = (FIFO_Cache *)malloc(sizeof(FIFO_Cache));
    cache->blocks = (CacheBlock *)calloc(block_count, sizeof(CacheBlock));
    cache->block_size = block_size;
    cache->block_count = block_count;
    cache->head = 0;
    cache->tail = 0;
    return cache;
}

void fifo_cache_store(FIFO_Cache *cache, off_t offset, const void *data) {
    memcpy(cache->blocks[cache->tail].data, data, cache->block_size);
    cache->blocks[cache->tail].offset = offset;
    cache->blocks[cache->tail].valid = 1;
    cache->tail = (cache->tail + 1) % cache->block_count;
    if (cache->tail == cache->head) {
        cache->head = (cache->head + 1) % cache->block_count;
    }
}

CacheBlock* fifo_cache_lookup(FIFO_Cache *cache, off_t offset) {
    for (size_t i = 0; i < cache->block_count; i++) {
        if (cache->blocks[i].valid && cache->blocks[i].offset == offset) {
            return &cache->blocks[i];
        }
    }
    return NULL;
}

void fifo_cache_free(FIFO_Cache *cache) {
    if (!cache) return;

    for (size_t i = 0; i < cache->block_count; i++) {
        if (cache->blocks[i].data) {
            free(cache->blocks[i].data);
            cache->blocks[i].data = NULL;
        }
    }
    free(cache->blocks);
    cache->blocks = NULL;
    free(cache);
    cache = NULL; 
}

