#include "arlloc.hpp"


void* Arlloc::alloc(std::size_t size) {

    if (size < BUFFER_SIZE && this->regions.is_empty() && this->free_blocks.is_empty()) {
        printf("\x1B[32m[INFO]:\033[0m\t Making new Region...\n");
        Region* region = Region::init();
        this->regions.push_back(region);
        return region->alloc(size);
    } else {

        // std::optional<Node<Block*>*> first = this->free_blocks.first();
        // if (!first.has_value()) {
            
        // }

        return nullptr;
    }

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

