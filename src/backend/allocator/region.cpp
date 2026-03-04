#include "lib.hpp"
#include "./allocator/region.hpp"

Region* Region::init()  {
    void* mem = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    if (mem == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
    return new(mem) Region();
}

void Region::drop(Region* region) {
    region->~Region();
    munmap(region, PAGE_SIZE);
}

void* Region::mnb(std::size_t size) {
    /**
     * Region
     *
     *   56 bytes (metadata)           4040 bytes (available buffer)
     *  +-------------------+-------------------------------------------------------------+
     *  |                   |                                                             |
     *  | buffer (8 bytes)  |+----------+------------------+----------+------------------+|
     *  | size   (8 bytes)  ||  Block   |   user data      |  Block   |   free block     ||
     *  | offset (8 bytes)  || (header) |   (size bytes)   | (header) |   (size bytes)   ||
     *  | next   (8 bytes)  |+----------+------------------+----------+------------------+|
     *  | blocks (24bytes)  |^          ^                                                 |
     *  |                   |offset     offset + sizeof(Block)                            |
     *  +-------------------+-------------------------------------------------------------+
     *  ^                   ^                   
     *  *region             *buffer             
     */
    if (size == 0) {
        return nullptr;
    }

    std::size_t aligned = ALIGN(this->offset, 8);

    if (aligned + sizeof(Block) + size > this->size) {
        return nullptr;
    }

    Block* block = (Block*)(this->buffer + aligned);
    block->size = size;
    block->is_free = false;

    void* ptr = (unsigned char*)block + sizeof(Block);

    /**
     * Region
     *
     *   56 bytes (metadata)           4040 bytes (available buffer)
     *  +-------------------+-------------------------------------------------------------+
     *  |                   |                                                             |
     *  | buffer (8 bytes)  |+----------+------------------+----------+------------------+|
     *  | size   (8 bytes)  ||  Block   |   user data      |  Block   |   free block     ||
     *  | offset (8 bytes)  || (header) |   (size bytes)   | (header) |   (size bytes)   ||
     *  | next   (8 bytes)  |+----------+------------------+----------+------------------+|
     *  | blocks (24bytes)  |^                             ^                              |
     *  |                   |block                         offset                         |
     *  +-------------------+-------------------------------------------------------------+
     *  ^                   ^                   
     *  *region             *buffer             
     */
    this->offset = aligned + sizeof(Block) + size;

    this->blocks.push_back(block);
    return ptr;
}

void* Region::alloc(std::size_t size) {
    if (this->blocks.is_empty()) {
        return this->mnb(size);
    }
}

std::size_t Region::total_region_size() {
    return sizeof(Region);
}