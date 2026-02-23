#include "mem_region.h"

Region* make_region() {
    Region* region = (Region*) mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    if (region == MAP_FAILED) {
        printf("Error: mmap failed\n");
        return NULL;
    }

    /**
     * Region
     *
     *   32 bytes (metadata)           4064 bytes (available buffer)
     *  +--------------------------------------------------------------------+
     *  | buffer (8 bytes)  |                                                |
     *  | size   (8 bytes)  |              Allocatable Buffer                |
     *  | offset (8 bytes)  |                                                |
     *  | next   (8 bytes)  |                                                |
     *  +--------------------------------------------------------------------+
     *  ^                   ^
     *  *region             *buffer
     * 
     */
    region->buffer = (unsigned char*)(region + 1);
    region->size = PAGE_SIZE - sizeof(Region);
    region->offset = 0;
    region->next = NULL;

    return region;
}

void* alloc_into_region(Region* region, size_t size) {
    if (region == NULL || size == 0) {
        printf("Error: allocate memory failed\n");
        return NULL;
    }

    u64 algined = ALIGN(region->offset, 8);

    if (region->offset + size > region->size) {
        printf("Error: Insufficient space\n");
        return NULL;
    }

    /**
     * Region
     *
     *   32 bytes (metadata)           4064 bytes (available buffer)
     *  +--------------------------------------------------------------------+
     *  | buffer (8 bytes)  |                                                |
     *  | size   (8 bytes)  | +---------+----------------------------------+ |
     *  | offset (8 bytes)  | |  Block  |               Free               | |
     *  | next   (8 bytes)  | +---------+----------------------------------+ |
     *  |                   |                                                |
     *  +--------------------------------------------------------------------+
     *  ^                   ^           ^
     *  *region             *buffer     offset
     * 
     */
    void* ptr = region->buffer + region->offset;
    region->offset = algined + size;
    return ptr;

}