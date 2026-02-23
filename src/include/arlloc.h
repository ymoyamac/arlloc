#ifndef ARLLOC_H
#define ARLLOC_H

#include "lib.h"
#include "mem_block.h"
#include "mem_region.h"

typedef struct bump_arllocator_s BumpArllocator;

struct bump_arllocator_s
{
    size_t size;
    Dll* free_blocks;
    Dll* regions;
};

BumpArllocator* make_arllocator(void);
void* arlloc(size_t size_in_bytes);

#endif