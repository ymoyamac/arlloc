#include "arlloc.h"

BumpArllocator* make_arllocator() {
    BumpArllocator* arllocator = (BumpArllocator*)malloc(sizeof(BumpArllocator));

    if (arllocator == NULL) {
        printf("Error to create BumpArllocator\n");
        return NULL;
    }

    arllocator->size = 0;
    arllocator->free_blocks = make_list();
    arllocator->regions = make_list();

    return arllocator;
}

void* arlloc(BumpArllocator* arlloc, size_t size_in_bytes) {
    // Verificar si existen bloques libres, en caso de que si devolver un bloque lo suficientemente grande para almacenar los elementos
    Region* region = make_region();
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
     * 
     */
    if (size_in_bytes > PAGE_SIZE) {
        return NULL;
    } else {
        
        
        return NULL;
    }

}

