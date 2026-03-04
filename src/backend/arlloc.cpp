#include "arlloc.hpp"


void* Arlloc::arlloc(std::size_t size) {

    if (this->regions.is_empty()) {
        Region* region = Region::init();
        this->regions.push_back(region);
        return region->alloc(size);
    }

    std::optional<Node<Block*>*> first = this->free_blocks.first();
    if (!first.has_value()) {
        
    }
    
    return nullptr;

}

