#include "mem_block.h"

void fmtblock(Block* block) {
    char* is_free_w = block->is_free ? "true" : "false";
    char* next_block_w = block->next_block == NULL ? "NULL" : "->";
    printf("Block { is_free: %s, size: %zu, next_block: %s }\n", is_free_w, block->size, next_block_w);
}
