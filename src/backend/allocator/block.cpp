#include "./allocator/block.hpp"

std::size_t Block::total_block_size() {
    return sizeof(Block);
}