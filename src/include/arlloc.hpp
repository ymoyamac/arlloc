#pragma once

#include "allocator/region.hpp"

#define BUFFER_SIZE PAGE_SIZE - sizeof(Region) - sizeof(Block)

class Arlloc {
private:
    /**
     * List of all regions managed by this allocator.
     * Each region is a PAGE_SIZE mmap allocation.
     */
    LinkedList<Region*> regions;

    /**
     * List of free blocks available for reuse.
     * Populated when a block is deallocated or when a new region is created.
     */
    LinkedList<Block*> free_blocks;

    /**
     * Searches for a free block large enough to fit `size` bytes.
     * If a suitable block is found, performs block splitting to avoid wasting memory.
     * Returns nullptr if no suitable block exists.
     *
     * @param size  Number of bytes requested by the user.
     * @return      Pointer to the usable memory, or nullptr if not found.
     */
    void* find_free_block(usize size);

public:

    /**
     * Constructs the allocator with empty region and free block lists.
     */
    Arlloc() {
        Logger::info("Calling Arlloc constructor...");
    }

    /**
     * Destroys the allocator and releases all regions back to the OS via munmap.
     */
    ~Arlloc() {
        Logger::info("Calling Arlloc destructor...");

        /** Clear free_blocks first to avoid dangling pointers into mmap pages. */
        this->free_blocks.clear();

        std::optional<Node<Region*>*> iter = this->regions.first();
        while (iter.has_value()) {
            Node<Region*>* current = iter.value();
            iter = current->next.get();
            Region::drop(current->data);
        }

        /** Clear regions to release Node<Region*> heap memory cleanly.
         *  Region::drop already released the mmap pages, so data pointers
         *  inside each node are now dangling. clear() prevents ~LinkedList
         *  from accessing them during automatic destruction.
         */
        this->regions.clear();
    }
    /**
     * Allocates `size` bytes and returns a pointer to the usable memory.
     * First attempts to reuse a free block. If none is available, creates a new region.
     *
     * @param size  Number of bytes to allocate.
     * @return      Pointer to the allocated memory.
     */
    void* alloc(usize size);

    /**
     * Marks the block at `ptr` as free and adds it to the free block list.
     * The memory is not returned to the OS until the allocator is destroyed.
     *
     * @param ptr  Pointer previously returned by `alloc`.
     */
    void dealloc(void* ptr);
};