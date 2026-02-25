#ifndef MEM_REGION_H
#define MEM_REGION_H

#include "lib.h"
#include "dll.h"
#include "mem_block.h"

typedef struct region_s Region;

struct region_s {
    /**
     * Region
     *
     *  32 bytes (metadata)           4064 bytes (available buffer)
     * +--------------------------------------------------------------------+
     * | buffer (8 bytes)  |                                                |
     * | size   (8 bytes)  |              Allocatable Buffer                |
     * | offset (8 bytes)  |                                                |
     * | next   (8 bytes)  |                                                |
     * +--------------------------------------------------------------------+
     * 
     *                              Region
     * +----------+----------------------------------------------------------+
     * |          |  +-------+-----------+    +-------+-----------+          |
     * |  Header  |  | Block |           | -> | Block |           | ->       |
     * |          |  +-------+-----------+    +-------+-----------+          |
     * +----------+----------------------------------------------------------+
     * 
     */
    unsigned char* buffer;
    size_t size;
    size_t offset;
    Node* next; // -> Points to the next block
};

Region* make_region(void);
void* handle_buffer_region(Region* region, size_t size);
Block* make_block_into(Region* region, size_t size_in_bytes);

#endif