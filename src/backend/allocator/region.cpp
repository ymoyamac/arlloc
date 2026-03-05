#include "lib.hpp"
#include "./allocator/region.hpp"
#include "./allocator/block.hpp"

Region* Region::init() {
    printf("\x1B[32m[INFO]:\033[0m\t Making new Region...\n");
    printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");
    void* mem = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    printf("\x1B[32m[INFO]:\033[0m\t Region* {\x1B[33m%p\033[0m}\n", mem);
    if (mem == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
    /** Construct Region in-place at the start of the mmap page using placement new. */
    return new(mem) Region();
}

void Region::drop(Region* region) {
    if (region == nullptr) {
        return;
    }

    /**
     * Manually call the destructor on each block before releasing the page.
     * Blocks live inside the mmap buffer, not on the heap, so delete cannot be used.
     */
    std::optional<Node<Block*>*> iter = region->blocks.first();
    while (iter.has_value()) {
        iter.value()->data->~Block();
        iter = iter.value()->next.get();
    }

    /** Call Region destructor manually since it was constructed with placement new. */
    region->~Region();

    /** Release the entire mmap page back to the OS. */
    munmap(region, PAGE_SIZE);
}

std::size_t Region::total_region_size() {
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

void* Region::alloc(LinkedList<Block*>* free_blocks, std::size_t size) {
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

void* Region::mnb(LinkedList<Block*>* free_blocks, std::size_t size) {
    printf("\x1B[32m[INFO]:\033[0m\t Making new block...\n");
    if (size == 0) {
        return nullptr;
    }

    std::size_t aligned = ALIGN(this->offset, 8);
    printf("\x1B[32m[INFO]:\033[0m\t Aligned: %zu, Offset: %zu\n", aligned, this->offset);

    if (aligned + sizeof(Block) + size > this->size) {
        printf("\x1B[91m[ERROR]:\033[0m\t Insufficient space in region.\n");
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
    Block* block = new(this->buffer + aligned) Block();
    printf("\x1B[32m[INFO]:\033[0m\t Block* {\x1B[33m%p\033[0m}\n", (void*)block);
    block->size    = size;
    block->is_free = false;
    block->region  = this;

    /** User data starts immediately after the Block header. */
    void* ptr = (unsigned char*)block + sizeof(Block);
    block->user_data = ptr;

    printf("\x1B[32m[INFO]:\033[0m\t %s\n", block->to_string().c_str());
    printf("\x1B[32m[INFO]:\033[0m\t Block size: %zu bytes\n", sizeof(Block));
    printf("\x1B[32m[INFO]:\033[0m\t User data* {\x1B[33m%p\033[0m}\n", ptr);

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
    printf("\x1B[32m[INFO]:\033[0m\t New offset: %zu\n", this->offset);

    this->blocks.push_back(block);
    printf("\x1B[32m[INFO]:\033[0m\t Blocks: %s\n", this->blocks.to_string().c_str());
    printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");

    /** Register the remaining free space as a free block. */
    Block* free_block = this->mfb(ptr, size);
    free_blocks->push_back(free_block);

    return ptr;
}

Block* Region::mfb(void* ptr, std::size_t user_data_size) {
    std::size_t aligned = ALIGN(user_data_size, 8);
    printf("\x1B[32m[INFO]:\033[0m\t Creating free block at offset: %zu\n", this->offset);

    /**
     * Place the free Block header immediately after the user data using placement new.
     *
     *  ptr + aligned
     *  v
     *  +----------+------------------------------+
     *  |  Block   |   remaining free space       |
     *  | (header) |                              |
     *  +----------+------------------------------+
     */
    Block* block = new((unsigned char*)ptr + aligned) Block();
    printf("\x1B[32m[INFO]:\033[0m\t Free Block* {\x1B[33m%p\033[0m}\n", (void*)block);

    /** Free block size equals remaining buffer space from current offset to end. */
    block->size    = this->size - this->offset;
    block->is_free = true;
    block->region  = this;

    printf("\x1B[32m[INFO]:\033[0m\t Free %s\n", block->to_string().c_str());
    printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");
    return block;
}

void* Region::wis_offset(void) {
    return (void*)this->offset;
}