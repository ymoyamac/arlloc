#include "./allocator/block.hpp"
#include "./allocator/region.hpp"

std::size_t Block::total_block_size() {
    return sizeof(Block);
}

std::string Block::to_string(void) {
    std::ostringstream oss;
    oss << "Block {";
    oss << " metada_size: " << "\x1B[96m"  << "\"" << sizeof(Block) << " bytes\"" << "\033[0m";
    oss << ", is_free: "    << (this->is_free ? "\x1B[32mtrue\033[0m" : "\x1B[91mfalse\033[0m");
    oss << ", size: "       << this->size;
    oss << ", region: "     << "\x1B[33m0x" << std::hex << (uintptr_t)this->region    << "\033[0m";
    oss << ", user_data: "  << "\x1B[33m0x" << std::hex << (uintptr_t)this->user_data << "\033[0m";
    oss << " }";
    return oss.str();
}

std::optional<std::pair<Block*, Block*>> Block::split(Block* free_block) {
    printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");
    printf("\x1B[32m[INFO]:\033[0m\t Block Spliting\n");

    //TODO: checar si el user_data es menor al espacio tenemos que dividir el bloque
    //pero no vamos a dividir el bloque en caso de: El espacio sobrante es menor al
    //tamaño del header del bloque (32 bytes hasta este punto) porque no quedará
    //espacio para crear un bloque nuevo, si el espacio restante es de 33 bytes
    //no se puede crear un bloque por que tiene que estar alineado, tenemos que crear
    //bloques multiplos, ejem: si el usuario pide un byte para almacenar, yo de voy a
    //dar un bloque de (32bytes) + 4 bytes = 36 bytes, es lo mínimo

    printf("\x1B[32m[INFO]:\033[0m\t ===============================================================================================================\n");
    return std::nullopt;
}
