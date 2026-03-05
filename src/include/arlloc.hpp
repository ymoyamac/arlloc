#pragma once

#include "allocator/region.hpp"

#define BUFFER_SIZE PAGE_SIZE - sizeof(Region) - sizeof(Block)

class Arlloc {
private:
    void* find_free_block(std::size_t size);

public:
    Arlloc() {
        printf("\x1B[32m[INFO]:\033[0m\t Calling Arlloc constructor\n");
    }
    LinkedList<Block*> free_blocks;
    LinkedList<Region*> regions;

    void* alloc(std::size_t size);
    void dealloc(void);

};