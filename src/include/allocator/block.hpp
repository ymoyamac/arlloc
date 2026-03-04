#pragma once

#include <iostream>
#include "../dll/node.hpp"
#include "allocator/region.hpp"

class Region;

class Block {
public:
    /**
     * Block
     *
     *   32 bytes (metadata)                    User Data
     *  +--------------------------------------------------------------------+
     *  | is_free (1 bytes) |                                                |
     *  | size    (8 bytes) |              Allocatable Buffer                |
     *  | region  (x bytes) |                                                |
     *  +--------------------------------------------------------------------+
     *  ^                   ^
     *  *block              *user_data
     * 
     */
    bool is_free;
    std::size_t size;
    Region* region;
    void* user_data;

    Block() : is_free(true), size(0), region(nullptr), user_data(nullptr)
    {
        printf("\x1B[32m[INFO]:\033[0m\t Calling block constructor\n");
    }

    std::size_t total_block_size(void);
    std::string to_string(void);
    
};