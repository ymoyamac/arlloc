#pragma once

#include <iostream>
#include "lib.hpp"
#include "node.hpp"

class Region {
private:
    std::size_t size = PAGE_SIZE;
    std::size_t buffer_size = 0;
    std::size_t free_size = 0;
    std::unique_ptr<Node<Region>> next;
};