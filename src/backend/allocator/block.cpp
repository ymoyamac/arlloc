#include "./allocator/block.hpp"
#include "./allocator/region.hpp"

std::size_t Block::total_block_size() {
    return sizeof(Block);
}

std::string Block::to_string(void) {
    std::ostringstream oss;
    oss << "Block {";
    oss << " is_free: "  << (this->is_free ? "\x1B[32mtrue\033[0m" : "\x1B[91mfalse\033[0m");
    oss << ", size: "    << this->size;
    oss << ", region: "  << "\x1B[33m0x" << std::hex << (uintptr_t)this->region   << "\033[0m";
    oss << ", user_data: "    << "\x1B[33m0x" << std::hex << (uintptr_t)this->user_data << "\033[0m";
    oss << " }";
    return oss.str();
}