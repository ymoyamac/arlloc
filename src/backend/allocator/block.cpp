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

    if (free_blocks.is_empty()) {
        return std::nullopt;
    }

    /**
     * Build a map with two keys per free block: start address and end address.
     * This allows O(1) lookup in both directions (forward and backward).
     *
     * start → Block*  (to find if block_ptr's end matches a free block's start)
     * end   → Block*  (to find if block_ptr's start matches a free block's end)
     *
     *  +----------+----------+          +----------+----------+
     *  |  Block   | userdata |          |  Block   | userdata |
     *  | (header) |          |          | (header) |          |
     *  +----------+----------+          +----------+----------+
     *  ^                     ^          ^                     ^
     *  start                 end        start                 end
     */
    Node<Block*>* iter = free_blocks.first().value();
    std::unordered_map<void*, Block*> umap;

    while (iter != nullptr) {
        Block* b  = iter->data;
        void* start = (void*)b;
        void* end   = (unsigned char*)b + sizeof(Block) + b->size;
        umap.insert({start, b});
        umap.insert({end,   b});
        iter = iter->next.get();
    }

    Logger::info("Block to deallocated block { \x1B[33m%p\033[0m }", (void*)block_ptr);
    Logger::info("free blocks {");
    for (const auto& [key, value] : umap) {
        Logger::info("  { \"%p\" -> \"%p\" }", key, (void*)value);
    }
    Logger::info("}");

    /**
     * rhs_addr: address immediately after block_ptr's user data.
     *           If a free block starts here, we can merge forward.
     *
     *  block_ptr              rhs_addr
     *  +----------+----------+----------+----------+
     *  |  block   | userdata |  rhs     | userdata |
     *  | (header) |          | (header) |          |
     *  +----------+----------+----------+----------+
     *
     * lhs_addr: address of block_ptr itself.
     *           If a free block ends here, we can merge backward.
     *
     *  lhs_addr (== block_ptr)
     *  +----------+----------+----------+----------+
     *  |  lhs     | userdata |  block   | userdata |
     *  | (header) |          | (header) |          |
     *  +----------+----------+----------+----------+
     */
    void* rhs_addr = (unsigned char*)block_ptr + sizeof(Block) + block_ptr->size;
    void* lhs_addr = (void*)block_ptr;

    Block* rhs = umap.contains(rhs_addr) ? umap.at(rhs_addr) : nullptr;
    Block* lhs = umap.contains(lhs_addr) ? umap.at(lhs_addr) : nullptr;

    // Ensure adjacent blocks belong to the same region to avoid cross-region corruption
    if (rhs != nullptr && rhs->region != block_ptr->region) rhs = nullptr;
    if (lhs != nullptr && lhs->region != block_ptr->region) lhs = nullptr;

    if (rhs != nullptr && lhs != nullptr) {
        /**
         * Both sides are free — merge all three into lhs.
         *
         *  Before:
         *  +----------+----------+----------+----------+----------+----------+
         *  |  lhs     | lhs data |  block   | blk data |  rhs     | rhs data |
         *  | (header) |          | (header) |          | (header) |          |
         *  +----------+----------+----------+----------+----------+----------+
         *
         *  After:
         *  +----------+--------------------------------------------------+
         *  |  lhs     |             merged free space                    |
         *  | (header) |                                                  |
         *  +----------+--------------------------------------------------+
         *
         *  new_size = lhs_header + lhs_data + block_header + block_data + rhs_header + rhs_data
         */
        Logger::info("Merging both sides (lhs + block + rhs)...");
        usize new_size = lhs->size + sizeof(Block) + block_ptr->size + sizeof(Block) + rhs->size;
        lhs->size    = new_size;
        lhs->is_free = true;
        free_blocks.pop_at(rhs);
        free_blocks.pop_at(lhs);
        return std::optional{lhs};
    }

    if (rhs != nullptr) {
        /**
         * Only forward neighbor is free — merge block_ptr with rhs.
         *
         *  Before:
         *  +----------+----------+----------+----------+
         *  |  block   | blk data |  rhs     | rhs data |
         *  | (header) |          | (header) |          |
         *  +----------+----------+----------+----------+
         *
         *  After:
         *  +----------+------------------------------+
         *  |  block   |       merged free space      |
         *  | (header) |                              |
         *  +----------+------------------------------+
         *
         *  new_size = block_data + rhs_header + rhs_data
         */
        Logger::info("Merging forward (block + rhs)...");
        usize new_size = block_ptr->size + sizeof(Block) + rhs->size;
        block_ptr->size    = new_size;
        block_ptr->is_free = true;
        free_blocks.pop_at(rhs);
        return std::optional{block_ptr};
    }

    if (lhs != nullptr) {
        /**
         * Only backward neighbor is free — merge lhs with block_ptr.
         *
         *  Before:
         *  +----------+----------+----------+----------+
         *  |  lhs     | lhs data |  block   | blk data |
         *  | (header) |          | (header) |          |
         *  +----------+----------+----------+----------+
         *
         *  After:
         *  +----------+------------------------------+
         *  |  lhs     |       merged free space      |
         *  | (header) |                              |
         *  +----------+------------------------------+
         *
         *  new_size = lhs_data + block_header + block_data
         */
        Logger::info("Merging backward (lhs + block)...");
        usize new_size = sizeof(Block) + lhs->size + sizeof(Block) + block_ptr->size;
        lhs->size    = new_size;
        lhs->is_free = true;
        free_blocks.pop_at(lhs);
        return std::optional{lhs};
    }

    /** No adjacent free blocks found — block_ptr will be added as standalone. */
    Logger::info("No adjacent free blocks found, adding as standalone...");
    return std::nullopt;
}


