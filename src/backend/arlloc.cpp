#include "arlloc.hpp"

std::optional<Block*> Arlloc::find_free_block(usize size) {
    Logger::info("Looking for a free block...");
    Logger::info("Free Blocks: %s", this->free_blocks.to_string().c_str());
    /** First allocation: no regions exist yet. */
    if (this->regions.is_empty() && this->free_blocks.is_empty()) {
        Logger::info("There is no free blocks available...");
        return std::nullopt;
    }

    /**
     * There is no free_block available
     */
    if (!this->free_blocks.first().has_value()) {
        return std::nullopt;
    }

    /**
     * At this point, there is at least one free block in the list of free blocks
     */
    Node<Block*>* iter = this->free_blocks.first().value();

    while(iter != nullptr) {
        Block* candidate = iter->data;
        Logger::info("Memory requested by the user \x1B[96m\"%zu bytes\"\033[0m, free bytes: \x1B[96m\"%llu bytes\"\033[0m",size, candidate->size);

        if (!candidate->is_free) {
            /** 
             * Defensive check: block should not be in free_blocks with is_free=false.
             * This happens because pop_at is not yet implemented.
             * TODO: remove with pop_at once available.
             */
            iter = iter->next.get();
            continue;
        }

        if (candidate->size < size) {
            /** Block is too small for this request, keep looking. */
            iter = iter->next.get();
            continue;
        }

        if (candidate->size == size) { //-> Probar este caso
            /**
             * Case 1: exact fit, reuse the block directly without splitting.
             *
             *  +----------+------------------+
             *  |  Block   |   user data      |
             *  | is_free  |   (size bytes)   |
             *  +----------+------------------+
             *       ↓
             *  +----------+------------------+
             *  |  Block   |   user data      |
             *  | !is_free |   (size bytes)   |
             *  +----------+------------------+
             */
            Logger::info("\x1B[33mNo way! These blocks are the same size\033[0m\t");
            candidate->is_free = false;
            free_blocks.pop_at(candidate);
            Logger::info("Free Blocks: %s", this->free_blocks.to_string().c_str());
            return std::optional{candidate};
        }

        if (candidate->size > size) {
            /**
             * If the free block is larger than the user space, split the block
             * and return the pointer to the user data on the left side of the pair
             * and return the pointer to the rest of the free block on the right side
             * of the pair.
             * 
             * Case 2: block is larger than requested, split it.
             * Left side goes to the user, right side stays in free_blocks.
             *
             *  Before:
             *  +----------+--------------------------------------------+
             *  |  Block   |         candidate->size bytes              |
             *  +----------+--------------------------------------------+
             *
             *  After split:
             *  +----------+------------+----------+--------------------+
             *  |  Block   | user data  |  Block   |  remaining bytes   |
             *  | !is_free | size bytes | is_free  |                    |
             *  +----------+------------+----------+--------------------+
             *  ^                       ^
             *  first                   second
             */
            std::optional<std::pair<Block*, Block*>> tupla = Block::split(iter->data, size);
            Logger::info("Free Blocks: %s", this->free_blocks.to_string().c_str());
            this->free_blocks.pop_at(iter->data);
            if (tupla == std::nullopt) {
                Logger::error("Block split failed");
                return std::nullopt;
            }
            if (tupla.value().second == nullptr) {
                /** Not enough remaining space for a new block, discard the leftover. */
                Logger::info("Leftover too small, discarded");
            } else {
                this->free_blocks.push_back(tupla.value().second);
                Logger::info("Free Blocks: %s", this->free_blocks.to_string().c_str());
            }
            return std::optional{tupla.value().first};
        }
    }

    Logger::info("Let's make new region...");
    return nullptr;
}

void* Arlloc::alloc(usize size) {
    std::optional<Block*> free_block = this->find_free_block(size);
    if (free_block.has_value()) {
        free_block.value()->region->get_blocks()->push_back(free_block.value());
        Logger::info("Blocks: %s", free_block.value()->region->get_blocks()->to_string().c_str());
        void* free_block_buffer = (unsigned char *)free_block.value() + sizeof(Block);
        return free_block_buffer;
    }
    /** No suitable free block found: create a new region. */
    Region* region = Region::init();
    this->regions.push_back(region);
    Logger::info("Regions: %s", this->regions.to_string().c_str());
    Logger::divider();
    void* raw_pointer = region->alloc(&this->free_blocks, size);
    return raw_pointer;
}

void Arlloc::dealloc(void* ptr) {

    //TODO: Llammar a munmap cuando todos los bloque esten libres de una region
    
    if (ptr == nullptr) return;
    Logger::info("Arlloc::dealloc");
    Logger::info("Available free blocks: %s", this->free_blocks.to_string().c_str());

    /**
     * Recover the Block header by stepping back sizeof(Block) bytes from the user pointer.
     *
     *  ptr
     *  +-----------+------------------+
     *  |   Block   |   user data      |
     *  | (header)  |                  |
     *  +-----------+------------------+
     *  ^           ^
     *  block       ptr
     */
    Block* block = (Block*)((unsigned char*)ptr - sizeof(Block));

    if (block->is_free) {
        Logger::error("Double free detected at \x1B[33m%p\033[0m", (void*)block);
        return;
    }

    block->is_free   = true;
    block->user_data = nullptr;
    block->region->get_blocks()->pop_at(block);

    std::optional<Block*> merged = Block::merge(this->free_blocks, block);
    if (merged.has_value()) {
        Logger::info("Merged block { \x1B[33m%p\033[0m, size: \x1B[96m\"%zu bytes\"\033[0m }",
            (void*)merged.value(), merged.value()->size);
        this->free_blocks.push_back(merged.value());
    } else {
        this->free_blocks.push_back(block);
    }

    Logger::info("Free %s", this->free_blocks.to_string().c_str());
    Logger::info("Deallocated block at \x1B[33m%p\033[0m, size: \x1B[96m\"%zu bytes\"\033[0m", (void*)block, block->size);
    Logger::divider();
}