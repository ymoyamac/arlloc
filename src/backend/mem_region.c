#include "mem_region.h"

Region* make_region() {
    Region* region = (Region*)mmap(NULL, PAGE_SIZE, PROT_RW, MAP_FLAGS, -1, 0);
    if (region == MAP_FAILED) {
        printf("Error: mmap failed\n");
        return NULL;
    }

    /**
     * Region
     *
     *   32 bytes (metadata)           4064 bytes (available buffer)
     *  +--------------------------------------------------------------------+
     *  | buffer (8 bytes)  |                                                |
     *  | size   (8 bytes)  |              Allocatable Buffer                |
     *  | offset (8 bytes)  |                                                |
     *  | next   (8 bytes)  |                                                |
     *  +--------------------------------------------------------------------+
     *  ^                   ^
     *  *region             *buffer
     * 
     */
    region->buffer = (unsigned char*)(region + 1);
    region->size = PAGE_SIZE - sizeof(Region);
    region->offset = 0;
    region->next = NULL;

    return region;
}

/**
 * Returns a new pointer of the buffer plus offset
 */
void* handle_buffer_region(Region* region, size_t size) {
    if (region == NULL || size == 0) {
        printf("Error: allocate memory failed\n");
        return NULL;
    }

    u64 algined = ALIGN(region->offset, 8);

    if (region->offset + size > region->size) {
        printf("Error: Insufficient space\n");
        return NULL;
    }

    /**
     * Region
     *
     *   32 bytes (metadata)           4064 bytes (available buffer)
     *  +--------------------------------------------------------------------+
     *  | buffer (8 bytes)  |                                                |
     *  | size   (8 bytes)  |++---------+-------*--------------------------+ |
     *  | offset (8 bytes)  |p|  Block  |  size *       Free               | |
     *  | next   (8 bytes)  |++---------+-------*--------------------------+ |
     *  |                   |                                                |
     *  +--------------------------------------------------------------------+
     *  ^                   ^                   ^
     *  *region             *buffer             offset
     * 
     */
    void* ptr = region->buffer + region->offset;
    region->offset = algined + sizeof(Block) + size; // -> Comentario: Aquí hay que sumar el tamaño del bloque para que el offset no sobrescriba el header del bloque?
    return ptr;

}

Block* make_block_into(Region* region, size_t size_in_bytes) {
    Block* buffer_region = (Block*)handle_buffer_region(region, size_in_bytes);
    buffer_region->is_free = false;
    buffer_region->size = sizeof(Block) + size_in_bytes;
    buffer_region->next_block = NULL;
    fmt_block(buffer_region);

    /**
     * Block
     *
     *  20 bytes (metadata)           X bytes (available buffer)
     * +---------------------------------------------------------------------+
     * | is_free (4 bytes)  |                                                |
     * | size    (8 bytes)  |              Allocatable Buffer                |
     * | next    (8 bytes)  |                                                |
     * +--------------------*------------------------------------------------+
     * ^                    ^                                                ^
     * *buffer_region       *buffer_region + 1                               offset
     */
    return buffer_region + 1; // -> Comentario: Hay que sumarle uno a la region del buffer porque el puntero apunta al principio del bloque?, osea que una "unidad" es del tamaño del bloque? Si se esta devolviendo correctamente el puntero para no sobreescribir el header del bloque?
    //return buffer_region + sizeof(Block);
}