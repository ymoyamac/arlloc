#ifndef MEM_BLOCK_H
#define MEM_BLOCK_H

#include "lib.h"

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
     */
    bool is_free;
    size_t size;
    Node* next_block;
};

void fmtblock(Block* block);


#endif