#include "arlloc.hpp"

void* Arlloc::find_free_block(std::size_t size) {
    /** First allocation: no regions exist yet. */
    if (this->regions.is_empty()) {
        printf("\x1B[32m[INFO]:\033[0m\t BUFFER_SIZE: %llu bytes\n", BUFFER_SIZE);
        printf("\x1B[32m[INFO]:\033[0m\t the memory requested by the user \x1B[96m\"%zu bytes\"\033[0m is smaller than the buffer: \x1B[96m\"%llu bytes\"\033[0m\n", size, BUFFER_SIZE);
        return nullptr;
    }

    if (!this->free_blocks.first().has_value()) {
        return nullptr;
    }

    
    return nullptr;
}

void* Arlloc::alloc(std::size_t size) {
    void* free_block = this->find_free_block(size);
    if (free_block != nullptr) {
        return free_block;
    }
    /** No suitable free block found: create a new region. */
    Region* region = Region::init();
    this->regions.push_back(region);
    return region->alloc(&this->free_blocks, size);
}

void Arlloc::dealloc(void* ptr) {
    if (ptr == nullptr) return;

    /**
     * Recover the Block header by stepping back sizeof(Block) bytes from the user pointer.
     *
     *  ptr
     *  +-----------+------------------+
     *  |   Block   |   user data      |
     *  | (header)  |                  |
     *  +-----------+------------------+
     *  ^           ^
     *  block       ptr
     */
    Block* block = (Block*)((unsigned char*)ptr - sizeof(Block));
    block->is_free = true;
    block->user_data = nullptr;
    this->free_blocks.push_back(block);

    printf("\x1B[32m[INFO]:\033[0m\t Deallocated block at \x1B[33m%p\033[0m, size: %zu bytes\n",
        (void*)block, block->size);
}