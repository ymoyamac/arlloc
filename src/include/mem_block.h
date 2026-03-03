#ifndef MEM_BLOCK_H
#define MEM_BLOCK_H

#include "lib.h"
#include "dll.h"

typedef struct block_s Block;

struct block_s {
    /**
     * Block
     *
     *  20 bytes (metadata)           X bytes (available buffer)
     * +---------------------------------------------------------------------+
     * | is_free (4 bytes)  |                                                |
     * | size    (8 bytes)  |              Allocatable Buffer                |
     * | next    (8 bytes)  |                                                |
     * +---------------------------------------------------------------------+
     * ^                    ^
     *                      *buffer
     */
    bool is_free;
    size_t size;
    Node* next_block;
};

Block* make_block(size_t size);
char* fmt_block(Block* block);

#endif