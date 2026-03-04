#pragma once

#include <iostream>
#include "node.hpp"
#include "region.hpp"

class Block {
private:
    std::size_t size = 0;
    bool is_free = true;
    std::unique_ptr<Node<Region>> region = nullptr;

public:

    std::size_t total_size() {
        return sizeof(Block);
    }
    
};