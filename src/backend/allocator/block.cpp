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

    printf("\x1B[32m[INFO]:\033[0m\t Creating a new block from the free block...\n");
    printf("\x1B[32m[INFO]:\033[0m\t Block* {\x1B[33m%p\033[0m}\n", (void*)free);
    usize original_size = free->size;
    free->is_free   = false;
    free->size      = size;
    free->user_data = (unsigned char*)free + sizeof(Block);

    printf("\x1B[32m[INFO]:\033[0m\t %s\n", free->to_string().c_str());
    printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");

    std::pair<Block*, Block*> tupla;
    tupla.first = free;

    /**
     * Block splitting: if the remaining space after the allocation is large enough
     * to hold a Block header plus at least one byte, create a new free block.
     * Otherwise discard the leftover to avoid unusable fragments.
     */
    if ((original_size - sizeof(Block) - size) > (sizeof(Block) + MINIMUM_SIZE)) {
        Block* remaining = new((unsigned char*)free->user_data + size) Block();
        printf("\x1B[32m[INFO]:\033[0m\t Free Block* {\x1B[33m%p\033[0m}\n", (void*)remaining);
        remaining->is_free   = true;
        remaining->size      = original_size - sizeof(Block) - size;
        remaining->region    = free->region;
        remaining->user_data = nullptr;
        printf("\x1B[32m[INFO]:\033[0m\t Free New %s\n", remaining->to_string().c_str());
        printf("\x1B[32m[INFO]:\033[0m\t Free block size: %zu bytes\n", remaining->size);
        tupla.second = remaining;
        printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");
        return std::optional{tupla};
    }

    printf("\x1B[32m[INFO]:\033[0m\t There is sufficient space to create a block with the minimum size: 32 bytes + 4 bytes = 36 bytes\n");
    tupla.second = nullptr;
    printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");
    return std::optional{tupla};
}
