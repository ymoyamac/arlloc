# Arlloc (memory allocator wirtten in C)

# arlloc

A custom memory allocator written in C, implementing an **arena allocator** strategy as an alternative to the system's `malloc`/`free`.

## Overview

`arlloc` manages memory through two levels of abstraction: **Regions** and **Blocks**. Memory is requested directly from the operating system via `mmap` and distributed internally using a bump allocator strategy, making allocations extremely fast.

## Architecture

### Region

A Region is a large contiguous block of memory obtained from the OS. Each region is one page in size (4096 bytes), with 32 bytes reserved for metadata and 4064 bytes available as an allocatable buffer.

```
 32 bytes (metadata)           4064 bytes (available buffer)
+--------------------------------------------------------------------+
| buffer (8 bytes)  |                                                |
| size   (8 bytes)  |              Allocatable Buffer                |
| offset (8 bytes)  |                                                |
| next   (8 bytes)  |                                                |
+--------------------------------------------------------------------+
```

Regions are linked together as a list, allowing the allocator to grow when a region runs out of space.

```
                             Region
+----------+----------------------------------------------------------+
|          |  +-------+-----------+    +-------+-----------+          |
|  Header  |  | Block |           | -> | Block |           | ->       |
|          |  +-------+-----------+    +-------+-----------+          |
+----------+----------------------------------------------------------+
```

### Block

A Block represents an individual allocation inside a Region. Each block has 16 bytes of metadata and points to the next block in the region.

```
 16 bytes (metadata)          X bytes (available buffer)
+--------------------------------------------------------------------+
| size   (8 bytes)  |              Allocatable Buffer                |
| next   (8 bytes)  |                                                |
+--------------------------------------------------------------------+
```

## How it works

`arlloc` uses a **bump allocator** strategy: an `offset` pointer advances forward through the region's buffer with each allocation. When memory is requested, the allocator returns a pointer to the current offset and advances it by the requested size.

Memory is aligned to 8-byte boundaries using the `ALIGN` macro, ensuring compatibility across architectures.

```c
// Request a region from the OS
Region* region = mem_region();

// Allocate memory inside the region
void* ptr = alloc_into_region(region, size);
```

## Trade-offs

| | |
|---|---|
| ✅ Extremely fast allocations | No per-block `free` — memory is released per region |
| ✅ No fragmentation | Region size is fixed to PAGE_SIZE |
| ✅ Cache-friendly sequential layout | Not suitable for long-lived mixed allocations |

## Use cases

This pattern is well suited for:

- Game engines with frame-based allocation cycles
- Compilers and parsers that allocate during a single pass
- Embedded systems with predictable memory lifetimes
- Any context requiring many small, fast allocations with a well-defined lifecycle