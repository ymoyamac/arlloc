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

    /**
     * Mark the free block as used and set its size to exactly what was requested.
     *
     *  Before:
     *  +----------+--------------------------------------------+
     *  |  Block   |         free->size bytes                   |
     *  | (header) |                                            |
     *  +----------+--------------------------------------------+
     *
     *  After split:
     *  +----------+------------+----------+--------------------+
     *  |  Block   | user data  |  Block   |  remaining bytes   |
     *  | (header) | size bytes | (header) |                    |
     *  +----------+------------+----------+--------------------+
     *  ^                       ^
     *  first                   second
     */

    Logger::info("Creating a new block from the free block...");
    usize original_size = free->size;
    free->is_free   = false;
    free->size      = aligned_size;
    free->user_data = (unsigned char*)free + sizeof(Block);
    Logger::info("Making Writable Block std::pair::<\033[3;42;30mfrist\033[0m, second>");
    Logger::info("Block* { \x1B[33m%p\033[0m }", (void*)free);
    Logger::info("%s", free->to_string().c_str());

    std::pair<Block*, Block*> tupla;
    tupla.first = free;

    /**
     * Block splitting: if the remaining space after the allocation is large enough
     * to hold a Block header plus at least one byte, create a new free block.
     * Otherwise discard the leftover to avoid unusable fragments.
     */
    Logger::info("Making Free Block std::pair::<first, \033[3;42;30msecond\033[0m>");
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

std::optional<Block*> Block::merge(LinkedList<Block*>& free_blocks, Block* block_ptr) {

    Logger::info("Block Merging...");
    if (free_blocks.is_empty() || free_blocks.get_size() == 1) {
        /** There are no blocks to merge */
        return std::nullopt;
    }

    /**
     *  Merge:
     *  +----------+------------+----------+----------------------+
     *  |  Block   | free_block |  Block   |    free_block        |
     *  | (header) |  40 bytes  | (header) |     80 bytes         |
     *  +----------+------------+----------+----------------------+
     *  ^                       ^
     *  first                   second 
     * 
     *  Merge:
     *  +----------+----------------------------------------------+
     *  |  Block   |                free_block                    |
     *  | (header) |                 120 bytes                    |
     *  +----------+----------------------------------------------+
     *  ^
     *  *free_block 
     * 
     * The only restriction that the merge blocking algorithm may have is that
     * the free blocks to be combined must be physically next to each other.
     * Otherwise, it will not be possible to combine.
     * 
     * Both must belong to the same region. Otherwise, it will not be possible
     * to combine.
     */
    
    Node<Block*>* iter = free_blocks.first().value();
    std::unordered_map<void*, usize> umap;

    while (iter != nullptr) {
        umap.insert({(unsigned char*)iter->data, BLOCK_HEADER_SIZE + iter->data->size});
        iter = iter->next.get();
    }
    Logger::info("Block to deallocated block { \x1B[33m%p\033[0m }", (void*)block_ptr);
    Logger::info("free blocks {");
    auto print_key_value = [](const auto& key, const auto& value)
    {
        Logger::info("  {\"%p\", \"%d\"}", key, value);
    };

    for (const std::pair<const void*, usize>& n : umap) {
        print_key_value(n.first, n.second);
    }
    Logger::info("}");

    void* rhs = (unsigned char*)block_ptr + sizeof(Block) + block_ptr->size;
    Logger::info("rhs { \x1B[33m%p\033[0m }", rhs);
    
    if (umap.contains(rhs)) {
        Logger::info("Can merge blocks...");
        Block* new_free_block = new(rhs) Block(
            true,                           // block->is_free
            umap.at(rhs) + sizeof(Block) + block_ptr->size, // block->size
            block_ptr->region,              // block->region
            nullptr                         // block->user_data
        );
        free_blocks.push_back(new_free_block);
        return std::optional{new_free_block};
    }


    return std::nullopt;
}


