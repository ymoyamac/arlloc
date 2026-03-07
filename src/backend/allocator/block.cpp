#include "./allocator/block.hpp"
#include "./allocator/region.hpp"

usize Block::total_block_size() {
    return sizeof(Block);
}

std::string Block::to_string(void) {
    std::ostringstream oss;
    oss << "Block {";
    oss << " metada_size: " << "\x1B[96m"  << "\"" << sizeof(Block) << " bytes\"" << "\033[0m";
    oss << ", is_free: "    << (this->is_free ? "\x1B[32mtrue\033[0m" : "\x1B[91mfalse\033[0m");
    oss << ", size: "       << this->size;
    oss << ", region: "     << "\x1B[33m0x" << std::hex << (uintptr_t)this->region    << "\033[0m";
    oss << ", user_data: "  << "\x1B[33m0x" << std::hex << (uintptr_t)this->user_data << "\033[0m";
    oss << " }";
    return oss.str();
}

std::optional<std::pair<Block*, Block*>> Block::split(Block* free, usize size) {
    Logger::info("Block Spliting...");
    usize aligned_size = ALIGN(size);

    /// Mark the free block as used and set its size to exactly what was requested.
    ///
    ///  Before:
    ///  +----------+--------------------------------------------+
    ///  |  Block   |         free->size bytes                   |
    ///  | (header) |                                            |
    ///  +----------+--------------------------------------------+
    ///
    ///  After split:
    ///  +----------+------------+----------+--------------------+
    ///  |  Block   | user data  |  Block   |  remaining bytes   |
    ///  | (header) | size bytes | (header) |                    |
    ///  +----------+------------+----------+--------------------+
    ///  ^                       ^
    ///  first                   second

    Logger::info("Creating a new block from the free block...");
    usize original_size = free->size;
    free->is_free   = false;
    free->size      = aligned_size;
    free->user_data = (unsigned char*)free + sizeof(Block);
    Logger::info("Pair::First");
    Logger::info("Block* { \x1B[33m%p\033[0m }", (void*)free);
    Logger::info("%s", free->to_string().c_str());

    std::pair<Block*, Block*> tupla;
    tupla.first = free;

    /**
     * Block splitting: if the remaining space after the allocation is large enough
     * to hold a Block header plus at least one byte, create a new free block.
     * Otherwise discard the leftover to avoid unusable fragments.
     */
    Logger::info("Pair::Second");
    if ((original_size - sizeof(Block) - aligned_size) > (sizeof(Block) + MINIMUM_SIZE)) {
        Block* remaining = new((unsigned char*)free->user_data + aligned_size) Block(
            true,                                           //block->is_free
            original_size - sizeof(Block) - aligned_size,   //block->size
            free->region,                                   //block->region
            nullptr                                         //block->user_data
        );
        tupla.second = remaining;
        return std::optional{tupla};
    }

    tupla.second = nullptr;
    Logger::divider();
    return std::optional{tupla};
}
