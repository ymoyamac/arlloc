#ifndef ARLLOC_H
#define ARLLOC_H

#include "lib.h"

typedef struct block_s Block;
typedef struct region_s Region;

Region* mem_region(void);
void* alloc_into_region(Region* region, size_t size);

#endif