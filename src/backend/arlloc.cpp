#include "arlloc.hpp"

void* Arlloc::find_free_block(std::size_t size) {
    /** It is the first time to user requests for memory */
    if (this->regions.is_empty()) {
        printf("\x1B[32m[INFO]:\033[0m\t BUFFER_SIZE: %llu bytes\n", BUFFER_SIZE);
        printf("\x1B[32m[INFO]:\033[0m\t the memory requested by the user \x1B[96m\"%zu bytes\"\033[0m is smaller than the buffer: \x1B[96m\"%llu bytes\"\033[0m\n", size, BUFFER_SIZE);
        return nullptr;
    }

    /**
     * At least one region with a free block already exists. We just need to check if the memory the user is requesting fits
     * within the user_data block of that free block; if not, we need to create a new region.
     * 
     */
    if (this->free_blocks.first().has_value()) {
        printf("\x1B[32m[INFO]:\033[0m\t =====================================================================================================================\n");
        printf("\x1B[32m[INFO]:\033[0m\t Looking for a free block...\n");
        Node<Block*>* firts_free_block = this->free_blocks.first().value();
        printf("\x1B[32m[INFO]:\033[0m\t First Free Block* {\x1B[33m%p\033[0m}\n", firts_free_block->data);
        printf("\x1B[32m[INFO]:\033[0m\t First Free %s\n", firts_free_block->data->to_string().c_str());
        printf("\x1B[32m[INFO]:\033[0m\t =====================================================================================================================\n");
        if (size < firts_free_block->data->size) {
            firts_free_block->data->is_free = false;
            void* ptr = (unsigned char*)firts_free_block->data + sizeof(Block);
            printf("\x1B[32m[INFO]:\033[0m\t User data* {\x1B[33m%p\033[0m}\n", ptr);
            // ->Aquí hay que hacer un block spliting, dividir el bloque de 3998 bytes en uno de sizeof(Block) + size = 32 bytes + size
            //y agregarlo a la lista de los bloque libres
            return ptr;
        }
        
    }

    return nullptr;
}

void* Arlloc::alloc(std::size_t size) {
    void* free_block = this->find_free_block(size);
    if (free_block != nullptr) {
        return free_block;
    }
    Region* region = Region::init();
    this->regions.push_back(region);
    return region->alloc(&this->free_blocks, size);

}

void Arlloc::dealloc() {
    std::optional<Node<Region*>*> iter = this->regions.first();
    if (!iter.has_value()) {
        return;
    }
    while(iter != nullptr) {
        Region::drop(iter.value()->data);
        iter = iter.value()->next.get();
    }
    
}

