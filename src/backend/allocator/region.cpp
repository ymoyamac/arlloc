#include "lib.hpp"
#include "./allocator/region.hpp"
#include "./allocator/block.hpp"

Region* Region::init() {
    void* mem = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    if (mem == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
    Logger::info("mmap* {\x1B[33m%p\033[0m}", mem);
    Logger::info("ALIGNMENT: \x1B[96m\"%d bytes\"\033[0m", ALIGNMENT);
    Logger::info("PAGE_SIZE: \x1B[96m\"%d bytes\"\033[0m", PAGE_SIZE);
    Logger::info("REGION_HEADER_SIZE: \x1B[96m\"%d bytes\"\033[0m", REGION_HEADER_SIZE);
    Logger::info("REGION_BUFFER_SIZE: \x1B[96m\"%d bytes\"\033[0m", REGION_BUFFER_SIZE);
    Logger::info("BLOCK_HEADER_SIZE: \x1B[96m\"%d bytes\"\033[0m", BLOCK_HEADER_SIZE);
    Logger::divider();
    /** Construct Region in-place at the start of the mmap page using placement new. */
    return new(mem) Region();
}

void Region::drop(Region* region) {
    Logger::info("Calling Drop Region* {\x1B[33m%p\033[0m}", (void*)region);

    if (region == nullptr) {
        return;
    }

    region->blocks.clear();

    /** Call Region destructor manually since it was constructed with placement new. */
    region->~Region();

    /** Release the entire mmap page back to the OS. */
    munmap(region, PAGE_SIZE);
    Logger::info("Returning memory to the operating system with \x1B[31mmunmap\033[0m");
    Logger::divider();
}

void* Region::alloc(LinkedList<Block*>* free_blocks, usize size) {
    void* ptr = this->mnb(free_blocks, size);
    /**
     * Region layout after allocation:
     *
     *   56 bytes (metadata)           4040 bytes (available buffer)
     *  +-------------------+-------------------------------------------------------------+
     *  |                   | 32 bytes                                                    |
     *  | buffer (8 bytes)  |+----------+------------------+----------+------------------+|
     *  | size   (8 bytes)  ||  Block   |   user data      |  Block   |   free space     ||
     *  | offset (8 bytes)  || (header) |   (size bytes)   | (header) |                  ||
     *  | blocks (24 bytes) |+----------+------------------+----------+------------------+|
     *  |                   |^          ^                  |                              |
     *  |                   |block      *ptr (returned)    |                              |
     *  +-------------------+-----------|-------------------------------------------------+
     *  ^                   ^           |                  ^
     *  *region             *buffer     |                  offset
     *                                  |
     *                                  Returns this pointer to the user
     */
    return ptr;
}

void* Region::mnb(LinkedList<Block*>* free_blocks, usize size) {
    if (size == 0) {
        return nullptr;
    }

    usize aligned_size = ALIGN(size);
    usize aligned_offset = ALIGN(this->offset);

    Logger::info("Aligned: %zu, Offset: %zu", aligned_offset, this->offset);

    if (aligned_offset + sizeof(Block) + aligned_size > this->size) {
        Logger::error("Insufficient space in region");
        return nullptr;
    }

    /**
     * Place a Block header at the current aligned offset using placement new.
     * Placement new ensures the constructor runs and fields are initialized.
     *
     *  buffer + aligned
     *  v
     *  +----------+------------------+
     *  |  Block   |   user data      |
     *  | (header) |   (size bytes)   |
     *  +----------+------------------+
     */
    Block* block = new(this->buffer + aligned_offset) Block(
        false,                                                          // block->is_free
        aligned_size,                                                   // block->size
        this,                                                           // block->region
        (unsigned char*)(this->buffer + aligned_offset) + sizeof(Block) //block->user_data
    );

    /** User data starts immediately after the Block header. */
    void* ptr = (unsigned char*)block + sizeof(Block);

    /**
     * Advance the offset past the Block header and user data.
     *
     *  Before:                         After:
     *  +----------+------------------+ +----------+------------------+--------+
     *  |  Block   |   user data      | |  Block   |   user data      |  free  |
     *  | (header) |                  | | (header) |                  |        |
     *  +----------+------------------+ +----------+------------------+--------+
     *  ^                               ^                              ^
     *  offset (before)                 block                          offset (after)
     */
    this->offset = aligned_offset + sizeof(Block) + aligned_size;

    this->blocks.push_back(block);
    Logger::info("Blocks: %s", this->blocks.to_string().c_str());
    Logger::divider();

    /** Register the remaining free space as a free block. */
    Block* free_block = this->mfb();
    free_blocks->push_back(free_block);
    return ptr;
}

Block* Region::mfb() {
    usize aligned_offset = ALIGN(this->offset);
    Logger::info("Creating free block at offset: %zu", aligned_offset);

    /**
     * Place the free Block header immediately after the user data using placement new.
     *
     *  ptr + aligned
     *  v
     *  +----------+------------------------------+
     *  |  Block   |   remaining free space       |
     *  | (header) |                              |
     *  +----------+------------------------------+
     * 
     * Free block size equals remaining buffer space from current offset to end
     */
    Block* block = new(this->buffer + aligned_offset) Block(
        true,                                         // block->is_free
        this->size - aligned_offset,                  // block->size
        this,                                         // block->region
        this->buffer + aligned_offset + sizeof(Block) // block->user_data  **remainin free space**
    );
    return block;
}

void* Region::wis_offset(void) {
    return (void*)this->offset;
}

usize Region::total_region_size() {
    return sizeof(Region);
}

std::string Region::to_string(void) {
    std::ostringstream oss;
    oss << "Region {";
    oss << " metada_size: " << "\x1B[96m" << "\"" << this->total_region_size() << " bytes\"" << "\033[0m";
    oss << ", buffer: "     << "\x1B[33m0x" << std::hex << (uintptr_t)this->buffer << "\033[0m";
    oss << ", size: "       << std::dec << this->size;
    oss << ", offset: "     << this->offset;
    oss << ", blocks: "     << std::dec << this->blocks.get_size();
    oss << " }";
    return oss.str();
}