#include "mem_block.h"

char* fmt_block(Block* block) {
    char* is_free_w = block->is_free ? "true" : "false";
    char* next_block_w = block->next_block == NULL ? "NULL" : "->";
    printf("Block { is_free: %s, size: %zu, next_block: %s }\n", is_free_w, block->size, next_block_w);
    return "";
}

Block* make_block(size_t size) {
    Block* block = (Block*)malloc(sizeof(Block));
    block->is_free = true;
    block->size = size;
    block->next_block = NULL;
    return block;
}
