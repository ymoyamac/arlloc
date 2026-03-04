#pragma once

#include <iostream>
#include "../dll/node.hpp"
#include "allocator/region.hpp"

class Region;

class Block {
public:
    bool is_free;
    std::size_t size;
    Region* region;

    Block() : is_free(true), size(0), region(nullptr)
    {
        printf("[INFO]:\t Calling block constructor\n");
    }

    std::size_t total_block_size();
    
};