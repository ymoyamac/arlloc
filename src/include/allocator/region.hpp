#pragma once

#include <iostream>
#include <sys/mman.h>
#include "lib.hpp"
#include "dll/node.hpp"
#include "dll/list.hpp"
#include "block.hpp"

class Block;

class Region {
private:
    /**
     * Pointer to the start of the allocatable buffer.
     * Points to the byte immediately after the Region metadata.
     */
    unsigned char* buffer;

    /**
     * Total number of bytes available in the buffer.
     * Equals PAGE_SIZE - sizeof(Region).
     */
    std::size_t size;

    /**
     * Current position within the buffer.
     * Advances forward with each new block allocation.
     */
    std::size_t offset;

    /**
     * List of all blocks allocated within this region.
     * Includes both used and free blocks.
     */
    LinkedList<Block*> blocks;

    /**
     * Constructs a Region in-place using placement new.
     * buffer points to the byte after the Region metadata within the same mmap page.
     *
     *  mmap page
     *  +---------------------+------------------------------------------+
     *  |  Region (metadata)  |  buffer (PAGE_SIZE - sizeof(Region))     |
     *  +---------------------+------------------------------------------+
     *  ^                     ^
     *  this                  this + sizeof(Region)
     */
    Region() :
        buffer((unsigned char*)this + sizeof(Region)),
        size(PAGE_SIZE - sizeof(Region)),
        offset(0)
    {
        printf("\x1B[32m[INFO]:\033[0m\t %s\n", this->to_string().c_str());
    }

    /**
     * Destroys the region's members before munmap releases the page.
     * Raw pointers are nulled to prevent dangling access.
     * The LinkedList destructor handles node cleanup automatically.
     */
    ~Region() {
        printf("\x1B[32m[INFO]:\033[0m\t Destroying Region { buffer: \x1B[33m%p\033[0m, size: %zu }\n",
            (void*)this->buffer, this->size);
        this->buffer = nullptr;
        this->size   = 0;
        this->offset = 0;
    }

    /**
     * Make new block (mnb).
     * Advances the offset and places a new Block header at the current position.
     * Also creates a free block for the remaining space after the allocation.
     *
     * @param free_blocks  The allocator's free block list to register the remaining space.
     * @param size         Number of bytes requested by the user.
     * @return             Pointer to the usable memory, or nullptr if insufficient space.
     */
    void* mnb(LinkedList<Block*>* free_blocks, std::size_t size);

    /**
     * Make free block (mfb).
     * Creates a free Block header at the remaining space after a new allocation.
     * The free block size equals the remaining buffer space from the current offset.
     *
     * @param ptr             Pointer to the start of the user data just allocated.
     * @param user_data_size  Size of the user data just allocated.
     * @return                Pointer to the new free Block.
     */
    Block* mfb(void* ptr, std::size_t user_data_size);

public:

    /**
     * Region layout:
     *
     *   56 bytes (metadata)           4040 bytes (available buffer)
     *  +--------------------------------------------------------------------+
     *  | buffer (8 bytes)  |                                                |
     *  | size   (8 bytes)  |              Allocatable Buffer                |
     *  | offset (8 bytes)  |                                                |
     *  | blocks (24 bytes) |                                                |
     *  +--------------------------------------------------------------------+
     *  ^                   ^
     *  *region             *buffer
     */

    /**
     * Allocates a new mmap page and constructs a Region at its start using placement new.
     * Returns a pointer to the Region, which lives at the beginning of the mmap page.
     *
     * @return  Pointer to the new Region, or throws std::runtime_error if mmap fails.
     */
    static Region* init();

    /**
     * Destroys the Region and releases the mmap page back to the OS.
     * Calls the Region destructor manually before munmap since placement new was used.
     *
     * @param region  Pointer to the Region to destroy.
     */
    static void drop(Region* region);

    /**
     * Returns the size of the Region metadata in bytes.
     *
     * @return  sizeof(Region).
     */
    static std::size_t total_region_size();

    /**
     * Returns a string representation of the region for debugging.
     *
     * @return  A formatted string with metadata, buffer address, size, offset and block count.
     */
    std::string to_string(void);

    /**
     * Allocates `size` bytes within this region.
     * Delegates to mnb to place a Block header and advance the offset.
     *
     * @param free_blocks  The allocator's free block list to register remaining space.
     * @param size         Number of bytes to allocate.
     * @return             Pointer to the usable memory.
     */
    void* alloc(LinkedList<Block*>* free_blocks, std::size_t size);

    /**
     * Returns the current offset as a void pointer.
     * Useful for debugging the current write position within the buffer.
     *
     * @return  Current offset cast to void*.
     */
    void* wis_offset(void);
};