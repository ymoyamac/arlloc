#pragma once

#include <iostream>
#include <sys/mman.h>
#include "lib.hpp"
#include "dll/node.hpp"
#include "dll/list.hpp"
#include "block.hpp"

class Block;

class Region {
private:
    unsigned char* buffer;
    std::size_t size;
    std::size_t offset;
    Node<Region>* next;
    LinkedList<Block*> blocks;

    Region() :
        buffer((unsigned char*)this + sizeof(Region)),
        size(PAGE_SIZE - sizeof(Region)),
        offset(0),
        next(nullptr)
    {
        printf("\x1B[32m[INFO]:\033[0m\t %s\n", this->to_string().c_str());
    }

    ~Region() {
        printf("\x1B[32m[INFO]:\033[0m\t Destroying region { buffer: \x1B[33m%p\033[0m, size: %zu }\n",
            (void*)this->buffer, this->size);

        // blocks es LinkedList<Block*>, la lista destruye sus nodos
        // pero los Block* apuntan al buffer de mmap, solo nulleamos
        this->buffer    = nullptr;
        this->next      = nullptr;
        this->size      = 0;
        this->offset    = 0;
    }

    void* mnb(std::size_t size);
    

public:

    /**
     * Region
     *
     *   56 bytes (metadata)           4040 bytes (available buffer)
     *  +--------------------------------------------------------------------+
     *  | buffer (8 bytes)  |                                                |
     *  | size   (8 bytes)  |              Allocatable Buffer                |
     *  | offset (8 bytes)  |                                                |
     *  | next   (8 bytes)  |                                                |
     *  | blocks (24bytes)  |                                                |
     *  +--------------------------------------------------------------------+
     *  ^                   ^
     *  *region             *buffer
     * 
     */

    static Region* init();
    static void drop(Region* region);
    static std::size_t total_region_size();
    std::string to_string(void);
    void* alloc(std::size_t);
    void* wis_offset(void);
};