#include "arlloc.hpp"

void* Arlloc::find_free_block(usize size) {
    Logger::info("Looking for a free block...");
    /** First allocation: no regions exist yet. */
    if (this->regions.is_empty() && this->free_blocks.is_empty()) {
        Logger::info("Memory requested by the user \x1B[96m\"%zu bytes\"\033[0m, free bytes: \x1B[96m\"%llu bytes\"\033[0m", size, BUFFER_SIZE);
        return nullptr;
    }

    /**
     * There is no free_block available
     */
    if (!this->free_blocks.first().has_value()) {
        return nullptr;
    }

    /**
     * At this point, there is at least one free block in the list of free blocks
     */
    Node<Block*>* iter = this->free_blocks.first().value();

    while(iter != nullptr) {
        Block* candidate = iter->data;

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


        if (candidate->size == size) {
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
            candidate->is_free = false;
            //TODO: pop_at el bloque libre
            return (unsigned char*)candidate + sizeof(Block);
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
             */
            //TODO: Arreglar la libreración de memoria
            std::optional<std::pair<Block*, Block*>> tupla = Block::split(iter->data, size);
            if (tupla == std::nullopt) {
                Logger::error("Block split failed");
                return nullptr;
            }
            if (tupla.value().second == nullptr) {
                /** Not enough remaining space for a new block, discard the leftover. */
                //TODO: pop_at del bloque libre, no es un bloque valido porque es
                //muy pequeño
                Logger::info("Leftover too small, discarded");
            } else {
                this->free_blocks.push_back(tupla.value().second);
            }
            //TODO: pop_at el bloque libre
            return (unsigned char*)tupla.value().first + sizeof(Block);
        }


    }

    Logger::info("Let's make new region...");
    return nullptr;
}

void* Arlloc::alloc(usize size) {
    void* free_block = this->find_free_block(size);
    if (free_block != nullptr) {
        return free_block;
    }
    /** No suitable free block found: create a new region. */
    Region* region = Region::init();
    this->regions.push_back(region);
    return region->alloc(&this->free_blocks, size);
}

void Arlloc::dealloc(void* ptr) {

    //TODO: Llammar a munmap cuando todos los bloque esten libres de una region
    
    //TODO: Agregar merge blocking, si un bloque queda libre a lado de otro
    //se juntan para hacer un bloque solo con la suma de ambos espacios libres
    //y se agrega a la lista de bloques libres
    if (ptr == nullptr) return;

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
    block->is_free   = true;
    block->user_data = nullptr;
    this->free_blocks.push_back(block);

    Logger::info("Deallocated block at \x1B[33m%p\033[0m, size: %zu bytes", (void*)block, block->size);
}