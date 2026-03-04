#include "lib.hpp"
#include "./allocator/region.hpp"

Region* Region::init()  {
    void* mem = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    printf("\x1B[32m[INFO]:\033[0m\t Region* {\x1B[33m%p\033[0m}\n", mem);
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
    printf("\x1B[32m[INFO]:\033[0m\t making new block...\n");
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
    printf("\x1B[32m[INFO]:\033[0m\t Aligned: %zu, Offset: %zu\n", aligned, this->offset);

    if (aligned + sizeof(Block) + size > this->size) {
        return nullptr;
    }

    Block* block = (Block*)(this->buffer + aligned);
    printf("\x1B[32m[INFO]:\033[0m\t Block* {\x1B[33m%p\033[0m}\n", block);
    block->size = size;
    block->is_free = false;

    void* ptr = (unsigned char*)block + sizeof(Block);

    printf("\x1B[32m[INFO]:\033[0m\t Block size: %zu bytes\n", sizeof(Block));
    printf("\x1B[32m[INFO]:\033[0m\t User data* {\x1B[33m%p\033[0m}\n", ptr);

    /**
     * Region
     *
     *   56 bytes (metadata)           4040 bytes (available buffer)
     *  +-------------------+-------------------------------------------------------------+
     *  |                   | 24 bytes                                                    |
     *  | buffer (8 bytes)  |+----------+------------------+----------+------------------+|
     *  | size   (8 bytes)  ||  Block   |   user data      |  Block   |   free block     ||
     *  | offset (8 bytes)  || (header) |   (size bytes)   | (header) |   (size bytes)   ||
     *  | next   (8 bytes)  |+----------+------------------+----------+------------------+|
     *  | blocks (24bytes)  |^                             |                              |
     *  |                   |block                         |                              |
     *  +-------------------+-------------------------------------------------------------+
     *  ^                   ^                              ^
     *  *region             *buffer                        offset
     */
    this->offset = aligned + sizeof(Block) + size;

    printf("\x1B[32m[INFO]:\033[0m\t Aligned: %zu, Offset: %zu\n", aligned, this->offset);

    this->blocks.push_back(block);
    printf("\x1B[32m[INFO]:\033[0m\t Blocks: %s\n", this->blocks.to_string().c_str());
    return ptr;
}

void* Region::alloc(std::size_t size) {
    if (this->blocks.is_empty()) {
        return this->mnb(size);
    }

    return NULL;
}

std::size_t Region::total_region_size() {
    return sizeof(Region);
}