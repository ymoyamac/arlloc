#include "lib.hpp"
#include "./allocator/region.hpp"
#include "./allocator/block.hpp"

Region* Region::init() {
    void* mem = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    if (mem == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
    Logger::info("mmap* {\x1B[33m%p\033[0m}", mem);
    Logger::info("PAGE_SIZE: %d bytes", PAGE_SIZE);
    Logger::info("REGION_HEADER_SIZE: %d bytes", REGION_HEADER_SIZE);
    Logger::info("REGION_BUFFER_SIZE: %d bytes", REGION_BUFFER_SIZE);
    Logger::info("BLOCK_HEADER_SIZE: %d bytes", BLOCK_HEADER_SIZE);
    Logger::divider();
    /** Construct Region in-place at the start of the mmap page using placement new. */
    return new(mem) Region();
}

void Region::drop(Region* region) {
    Logger::info("Calling Drop Region* {\x1B[33m%p\033[0m}", (void*)region);

    if (region == nullptr) {
        return;
    }

    /**
     * Manually call the destructor on each block before releasing the page.
     * Blocks live inside the mmap buffer, not on the heap, so delete cannot be used.
     */
    std::optional<Node<Block*>*> first = region->blocks.first();
    if (!first.has_value()) {
        return;
    }
    Node<Block*>* iterator = first.value();
    Logger::info("First Block of the Region* {\x1B[33m%p\033[0m}", (void*)iterator->data);

    while (iterator != nullptr) {
        Logger::info(iterator->data->to_string().c_str());
        iterator = iterator->next.get();
        region->blocks.pop_front();
    }

    /** Call Region destructor manually since it was constructed with placement new. */
    region->~Region();

    /** Release the entire mmap page back to the OS. */
    munmap(region, PAGE_SIZE);
    Logger::info("Returning memory to the operating system with munmap");
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

    usize aligned = ALIGN(this->offset, 8);
    Logger::info("Aligned: %zu, Offset: %zu", aligned, this->offset);

    if (aligned + sizeof(Block) + size > this->size) {
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
    Block* block = new(this->buffer + aligned) Block(
        false,  // block->is_free
        size,   // block->size
        this,   // block->region
        (unsigned char*)(this->buffer + aligned) + sizeof(Block) //block->user_data
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
    this->offset = aligned + sizeof(Block) + size;

    this->blocks.push_back(block);
    Logger::info("Blocks: %s", this->blocks.to_string().c_str());
    Logger::divider();

    /** Register the remaining free space as a free block. */
    Block* free_block = this->mfb(ptr, size);
    free_blocks->push_back(free_block);
    return ptr;
}

Block* Region::mfb(void* ptr, usize user_data_size) {
    usize aligned = ALIGN(user_data_size, 8);
    Logger::info("Creating free block at offset: %zu", this->offset);

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
    Block* block = new((unsigned char*)ptr + aligned) Block(true, this->size - this->offset, this, (unsigned char*)ptr + aligned + sizeof(Block));
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