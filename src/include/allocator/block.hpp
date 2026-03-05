#pragma once

#include <iostream>
#include <sstream>
#include "../dll/node.hpp"

class Region;

class Block {
public:
    /**
     * Block layout:
     *
     *   32 bytes (metadata)                    User Data
     *  +--------------------------------------------------------------------+
     *  | is_free    (1 byte)  |                                             |
     *  | size       (8 bytes) |              Allocatable Buffer             |
     *  | region     (8 bytes) |                                             |
     *  | user_data  (8 bytes) |                                             |
     *  +--------------------------------------------------------------------+
     *  ^                      ^
     *  *block                 *user_data
     */
    bool        is_free;
    std::size_t size;
    Region*     region;
    void*       user_data;

    /**
     * Constructs a Block with default values.
     * Intended to be used with placement new inside a Region buffer.
     */
    Block() : is_free(true), size(0), region(nullptr), user_data(nullptr) {
        printf("\x1B[32m[INFO]:\033[0m\t Calling block constructor\n");
    }

    /**
     * Destroys the block and nulls its pointers.
     * Does not free any memory — the Region buffer is released by munmap.
     */
    ~Block() {
        printf("\x1B[32m[INFO]:\033[0m\t Calling block destructor { size: %zu, region: \x1B[33m%p\033[0m }\n",
            this->size, (void*)this->region);
        this->region    = nullptr;
        this->user_data = nullptr;
    }

    /**
     * Returns the size of the Block metadata in bytes.
     *
     * @return  sizeof(Block).
     */
    std::size_t total_block_size(void);

    /**
     * Returns a string representation of the block for debugging.
     * Memory addresses are printed in yellow for readability.
     *
     * @return  A formatted string with all block fields.
     */
    std::string to_string(void);

    /**
     * Split one free block to get new block where the user will writes
     * his data and new next free_block
     */
    static std::optional<std::pair<Block*, Block*>> split(Block* free_block);

    static void* mnb(LinkedList<Block*>* free_blocks, std::size_t size);
};