#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "lib.hpp"
#include "../dll/node.hpp"
#include "../dll/list.hpp"

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
    usize       size;
    Region*     region;
    void*       user_data;

    /**
     * Constructs a Block with default values.
     * Intended to be used with placement new inside a Region buffer.
     */
    Block() : is_free(true), size(0), region(nullptr), user_data(nullptr) {
        Logger::info("Calling block constructor");
        Logger::info("Block* { \x1B[33m%p\033[0m }", (void*)this);
        Logger::info(this->to_string().c_str());
        Logger::divider();
    }

    Block(bool is_free, size_t size, Region* region, void* user_data)
        : is_free(is_free), size(size), region(region), user_data(user_data)
    {
        Logger::info("Calling block constructor args...");
        Logger::info("Block* { \x1B[33m%p\033[0m }", (void*)this);
        Logger::info(this->to_string().c_str());
        Logger::divider();
    }

    /**
     * Destroys the block and nulls its pointers.
     * Does not free any memory — the Region buffer is released by munmap.
     */
    ~Block() {
        Logger::info("Calling block destructor { size: %zu, region: \x1B[33m%p\033[0m }",this->size, (void*)this->region);
        this->region    = nullptr;
        this->user_data = nullptr;
    }

    /**
     * Returns the size of the Block metadata in bytes.
     *
     * @return  sizeof(Block).
     */
    usize total_block_size(void);

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
    static std::optional<std::pair<Block*, Block*>> split(Block* free_block, usize size);

    /**
     * Physically combines two adjacent free blocks
     * 
     * @return  The new free_block
     */
    static std::optional<Block*> merge(LinkedList<Block*>& free_blocks, Block* block_ptr);
};

constexpr usize BLOCK_HEADER_SIZE = sizeof(Block);
