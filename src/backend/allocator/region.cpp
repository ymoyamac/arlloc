#include "lib.hpp"
#include "./allocator/region.hpp"
#include "./allocator/block.hpp"

Region* Region::init()  {
    printf("\x1B[32m[INFO]:\033[0m\t Making new Region...\n");
    printf("\x1B[32m[INFO]:\033[0m\t =====================================================================================================================\n");
    //TODO: Agregar valores de PAGE_SIZE alineados a una potencia de dos: 4, 8, 12
    void* mem = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    printf("\x1B[32m[INFO]:\033[0m\t Region* {\x1B[33m%p\033[0m}\n", mem);
    if (mem == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
    return new(mem) Region();
}

void Region::drop(Region* region) {
    if (region == nullptr) {
        return;
    }
    std::optional<Node<Block*>*> iter = region->blocks.first();
    //TODO: liberar memoria de los bloques libres
    while (iter != nullptr) {
        iter.value()->data->~Block();
        iter = iter.value()->next.get();
    }
    region->~Region();
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
    oss << ", size: "   << std::dec << this->size;
    oss << ", offset: " << this->offset;
    oss << ", next: "   << "\x1B[33m0x" << std::hex << (uintptr_t)this->next   << "\033[0m";
    oss << ", blocks: " << std::dec << this->blocks.get_size();
    oss << " }";
    return oss.str();
}

/**
 * Returns the pointer to the empty space where the user can write their data
 */
void* Region::alloc(LinkedList<Block*>* free_blocks, std::size_t size) {
    void* ptr = this->mnb(free_blocks, size);
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
     *  | blocks (24bytes)  |^          ^                  |                              |
     *  |                   |block      *ptr               |                              |
     *  +-------------------+-----------|-------------------------------------------------+
     *  ^                   ^           |                  ^
     *  *region             *buffer     |                  offset
     *                                  |
     *                                  Returns this pointer
     */
    return ptr;
}

void* Region::mnb(LinkedList<Block*>* free_blocks, std::size_t size) {
    printf("\x1B[32m[INFO]:\033[0m\t making new block...\n");
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
    block->region = this;

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
     *  |                   |offset     ptr = offset + sizeof(Block)                      |
     *  +-------------------+-------------------------------------------------------------+
     *  ^                   ^                   
     *  *region             *buffer             
     */
    void* ptr = (unsigned char*)block + sizeof(Block);
    block->user_data = ptr;

    printf("\x1B[32m[INFO]:\033[0m\t %s\n", block->to_string().c_str());

    printf("\x1B[32m[INFO]:\033[0m\t Block size: %zu bytes\n", sizeof(Block));
    printf("\x1B[32m[INFO]:\033[0m\t User data* {\x1B[33m%p\033[0m}\n", ptr);

    /**
     * Region
     *
     *   56 bytes (metadata)           4040 bytes (available buffer)
     *  +-------------------+-------------------------------------------------------------+
     *  |                   | 32 bytes                                                    |
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
    printf("\x1B[32m[INFO]:\033[0m\t =====================================================================================================================\n");
    Block* free_block = this->mfb(ptr, size);
    free_blocks->push_back(free_block);
    return ptr;
}

Block* Region::mfb(void* ptr, std::size_t user_data_size) {
    std::size_t aligned = ALIGN(user_data_size, 8);
    printf("\x1B[32m[INFO]:\033[0m\t Aligned: %zu, Offset: %zu\n", aligned, this->offset);

    Block* block = (Block*)((unsigned char*)ptr + aligned);
    printf("\x1B[32m[INFO]:\033[0m\t Free Block* {\x1B[33m%p\033[0m}\n", block);
    block->size = PAGE_SIZE - sizeof(Region) - (this->blocks.get_size() * sizeof(Block)) - user_data_size;
    block->is_free = true;
    block->region = this;
    printf("\x1B[32m[INFO]:\033[0m\t Free %s\n", block->to_string().c_str());
    printf("\x1B[32m[INFO]:\033[0m\t =====================================================================================================================\n");
    return block;
}

void* Region::wis_offset(void) {
    return (void*) offset;
}