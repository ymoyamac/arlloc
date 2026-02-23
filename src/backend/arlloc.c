#include "arlloc.h"

BumpArllocator* make_arllocator() {
    BumpArllocator* arllocator = (BumpArllocator*)malloc(sizeof(BumpArllocator));

    if (arllocator == NULL) {
        printf("Error to create BumpArllocator\n");
        return NULL;
    }

    arllocator->size = 0;
    arllocator->free_blocks = NULL;
    arllocator->regions = NULL;

    return arllocator;
}

void* arlloc(size_t size_in_bytes) {
    BumpArllocator* arlloc = make_arllocator();
    Dll* regions = make_list();
    /**
     * Case 1: if the memory size of elements is greater than PAGE_SIZE
     * 
     * Example: 
     *  arlloc(sizeof(int) * 2000) // 8000 bytes
     *  
     * crear regiones de memoria hasta que se cubra el espacio solicitado, es decir,
     * tener dos regiones de memoria de 4064 con dos bloques internos de x bytes hasta
     * cada una.
     * 
     *                 Region                                                       Region                         
     * +----------+------------------------------+    +----------+---------------------------------------------------------+    
     * |          |  +-------+----------------+  |    |          |  +-------+----------------+  +-------+---------------+  |    
     * |  Header  |  | Block | is_free: false |  | -> |  Header  |  | Block | is_free: false |->| Block | is_free: true |  | -> 
     * |          |  +-------+----------------+  |    |          |  +-------+----------------+  +-------+---------------+  |    
     * +----------+------------------------------+    +----------+---------------------------------------------------------+    
     */
    if (size_in_bytes > PAGE_SIZE) {
        return NULL;
    } else {
        arlloc->regions = regions;
        /**
         * Case 2: is the memory size if less than PAGE_SIZE
         * 
         * Block
         *
         *  20 bytes (metadata)           X bytes (available buffer)
         * +---------------------------------------------------------------------+
         * | is_free (4 bytes)  |                                                |
         * | size    (8 bytes)  |              Allocatable Buffer                |
         * | next    (8 bytes)  |                                                |
         * +---------------------------------------------------------------------+
         * ^                    ^
         * *block               *buffer
         */
        Region* region = make_region();
        Block* buffer_region = (Block*)alloc_into_region(region, size_in_bytes);
        buffer_region->is_free = false;
        buffer_region->size = sizeof(Block) + size_in_bytes;
        buffer_region->next_block = NULL;
        fmtblock(buffer_region);
        push_front(regions, buffer_region, TYPE_ANY);

        return buffer_region + 1;
    }

}

