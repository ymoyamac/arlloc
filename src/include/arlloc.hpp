#pragma once

#include "allocator/region.hpp"

class Arlloc {
public:
    Arlloc() {
        printf("\x1B[32m[INFO]:\033[0m\t Calling Arlloc constructor\n");
    }
    LinkedList<Block*> free_blocks;
    LinkedList<Region*> regions;

    void* arlloc(std::size_t size);
    

};